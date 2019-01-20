#pragma once
#include <vapaee/bgbox/common.hpp>

using namespace eosio;
namespace vapaee {
    namespace bgbox {

        // auxiliar functions ---------------------------------------------------------        
        void collect_units_for_asset(
            name user,
            const slug_asset &quantity,
            vector<uint64_t> &units,
            vector<int> &quantities) {
            // ------------------------------

            // aux variables
            slug asset_slug = quantity.symbol.code().raw();
            item_asset asset;
            item_spec spec;
            get_item_spec_for_slug(asset_slug, asset, spec);

            // we search over owner's units of this asset. We put the first quantity.amount units id ina list
            uint64_t asset_id = asset.id;
            item_units units_table(get_self(), spec.app);
            int plus_quantity, listed;
            auto units_index = units_table.template get_index<"asset"_n>();
            auto units_itr = units_index.lower_bound(asset_id);
            for (listed=0; listed<quantity.amount && units_itr != units_index.end(); units_itr++) {
                if (units_itr->asset == asset_id) {
                    units.push_back(units_itr->id);
                    plus_quantity = units_itr->quantity;
                    if (listed + plus_quantity > quantity.amount) {
                        plus_quantity = quantity.amount - listed;
                    }
                    listed += plus_quantity;
                    quantities.push_back(plus_quantity);
                }
            }

            // we ensure 'from' has enough units to send
            eosio_assert(listed == quantity.amount, (string("Not enough items of asset ") + asset_slug.to_string()).c_str());
        }

        void allocate_slot_in_deposit(name owner, const slug_symbol& symbol, const item_spec &spec, item_unit &units) {
            // exigimos que la cantidad sea positiva. de otra manera no debería allocarse el slot
            eosio_assert(units.quantity > 0, "quantity must be positive in deposit slot allocation");

            // primero obtengo la instancia de container para el usuario
            container_instance deposit;
            container_asset asset;
            find_app_deposit(owner, spec.app, asset, deposit);

            containers cont_table(get_self(), owner.value);
            item_units units_table(get_self(), spec.app);
            auto unit_itr = units_table.begin();

            uint64_t slot_id;
            // si tiene slots vacios en lista
            if (deposit.slots.size() > 0) {
                // saco el primero (pop)
                slot_id = deposit.slots[deposit.slots.size()-1];
                cont_table.modify( deposit, same_payer, [&](auto &a) {
                    a.slots.pop_back();
                });
                
                // lo relleno con la info de units
                unit_itr = units_table.find(slot_id);
                units_table.modify( *unit_itr, same_payer, [&](auto &a) {
                    a.asset = units.asset;
                    a.quantity = units.quantity;
                });                
            } else {
                // la lista de slots es vacía
                // instancio un nuevo item_unit y le copio todo la info de units
                units_table.emplace( owner, [&](auto &a) {
                    a.id = units_table.available_primary_key();
                    a.owner = owner;
                    a.asset = units.asset;
                    a.quantity = units.quantity;
                    a.slot.container = deposit.id;
                    a.slot.position = a.id;
                });                
            }
        }        

        void find_inventory_slot(name owner, const slug_symbol& symbol, const item_spec &spec, item_unit & unit) {
            // finds an empty slot for the user's container instance of this app inventory. The slot remains empty

            // tengo que saber dar con la instancia de container del usuario para este tipo de item
            container_instance inventory;
            container_asset asset;
            find_app_inventory(owner, spec.app, asset, inventory);
            eosio_assert(inventory.empty > 0, "User does not have any space left in the app inventory");

            item_units units_table(get_self(), spec.app);
            uint64_t unit_id = 0;

            if (inventory.slots.size() > 0) {
                // tengo slots libres instanciados así que tomo el último y cargo la respuesta
                unit_id = inventory.slots[inventory.slots.size()-1];
                auto unit_itr = units_table.find(unit_id);
                unit.id = unit_id;
                unit.owner = owner;
                unit.slot.container = inventory.id;
                unit.slot.position = unit_itr->slot.position;
                unit.quantity = 0;
            } else {
                // tengo que pedir un iterador sobre los item_units de este container
                slotinfo slot;
                slot.container = inventory.id;
                slot.position = 0;
                auto unit_index = units_table.template get_index<"slot"_n>();
                auto unit_itr = unit_index.upper_bound(slot.to128bits());
                for ( ; unit_itr != unit_index.end(); unit_itr++) {
                    // para cada item_unit del inventario pedirle su posición y quedarme con la pos más baja+1
                    if (unit_itr->slot.container == inventory.id) {
                        slot.position = unit_itr->slot.position + 1;
                        break;
                    }
                }
                unit_itr = unit_index.find(slot.to128bits());
                eosio_assert(unit_itr == unit_index.end(), "find_inventory_slot slot.position misscalculation");

                // instancio un item_unit con el owner, id, slot.position encontrada, quantity = 0 
                unit_id = units_table.available_primary_key();
                units_table.emplace(owner, [&](auto &a){
                    a.id = unit_id;
                    a.owner = owner;
                    a.slot.container = inventory.id;
                    a.slot.position = slot.position;
                    a.quantity = 0;
                });

                // tengo que agregar el item_unit.id a la lista de cointainer.slots porque sigue vacío
                containers cont_table(get_self(), owner.value);
                cont_table.modify(inventory, owner, [&](auto &a){
                    a.slots.push_back(unit_id);
                });
            }                        
        }        

        void get_container_asset_for_slug(const slug &asset_slug, container_asset &asset) {
            // aux variables
            const char * error_str = (asset_slug.to_string() + " was not registered as container asset").c_str();
            // we search for the asset identityed by quantity.symbol (asset_slug)
            container_assets assets_table(get_self(), get_self().value);
            auto index = assets_table.template get_index<"slug"_n>();
            auto itr = index.find( asset_slug.to128bits() );
            eosio_assert(itr != index.end(), error_str);
            // we make sure we have the correct asset (avoiding 128 bit colusion)
            while (itr->supply.symbol.code().raw() != asset_slug && itr != index.end()) {
                itr++;
            }
            eosio_assert(itr != index.end(), error_str);
            eosio_assert(itr->supply.symbol.code().raw() == asset_slug, error_str);
            
            asset.id = itr->id;
            asset.supply = itr->supply;
            asset.spec = itr->spec;
            asset.publisher = itr->publisher;
            asset.block = itr->block;
        }

        void get_container_spec_for_slug(const slug &asset_slug, container_asset &asset, container_spec &spec) {
            container_specs specs_table(get_self(), get_self().value);
            get_container_asset_for_slug(asset_slug, asset);
            auto itr = specs_table.find(asset.spec);
            eosio_assert(itr != specs_table.end(), "asset point to a null spec");

            spec.id = itr->id;
            spec.nick = itr->nick;
            spec.app = itr->app;
        }

        void get_item_asset_for_slug(const slug &asset_slug, item_asset &asset) {
            // aux variables
            const char * error_str = (asset_slug.to_string() + " was not registered as item asset").c_str();
            // we search for the asset identityed by quantity.symbol (asset_slug)
            item_assets assets_table(get_self(), get_self().value);
            auto index = assets_table.template get_index<"slug"_n>();
            auto itr = index.find( asset_slug.to128bits() );
            eosio_assert(itr != index.end(), error_str);
            // we make sure we have the correct asset (avoiding 128 bit colusion)
            while (itr->supply.symbol.code().raw() != asset_slug && itr != index.end()) {
                itr++;
            }
            eosio_assert(itr != index.end(), error_str);
            eosio_assert(itr->supply.symbol.code().raw() == asset_slug, error_str);
            
            asset.id = itr->id;
            asset.supply = itr->supply;
            asset.max_supply = itr->max_supply;
            asset.spec = itr->spec;
            asset.publisher = itr->publisher;
            asset.block = itr->block;
        }

        void get_item_spec_for_slug(const slug &asset_slug, item_asset &asset, item_spec &spec) {
            item_specs specs_table(get_self(), get_self().value);
            get_item_asset_for_slug(asset_slug, asset);
            auto itr = specs_table.find(asset.spec);
            eosio_assert(itr != specs_table.end(), "asset point to a null spec");

            spec.id = itr->id;
            spec.nick = itr->nick;
            spec.app = itr->app;
            spec.maxgroup = itr->maxgroup;
        }

        void add_balance( name owner, slug_asset value, name ram_payer ) {
            accounts account_table( get_self(), owner.value );
            auto index = account_table.template get_index<"slug"_n>();
            auto itr = index.find( value.symbol.code().raw().to128bits());
            
            // we make sure we have the correct asset (avoiding 128 bit colusion)
            while ( itr != index.end() && itr->balance.symbol != value.symbol ) {
                itr++;
            }

            if( itr == index.end() ) {
                account_table.emplace( ram_payer, [&]( auto& a ){
                    a.balance = value;
                });
            } else {
                account_table.modify( *itr, same_payer, [&]( auto& a ) {
                    a.balance += value;
                });
            }  
        }
    
        void sub_balance( name owner, slug_asset value ) {
            accounts account_table( get_self(), owner.value );
            auto index = account_table.template get_index<"slug"_n>();
            auto itr = index.find( value.symbol.code().raw().to128bits());
            
            // we make sure we have the correct asset (avoiding 128 bit colusion)
            while ( itr != index.end() && itr->balance.symbol != value.symbol ) {
                itr++;
            }

            eosio_assert(itr != index.end(), (value.symbol.code().raw().to_string() + " was not registered as asset.").c_str() );
            eosio_assert(itr->balance <= value, (string("not enough funds in asset ") + value.symbol.code().raw().to_string()).c_str() );
            uint64_t final_balance = itr->balance.amount - value.amount;
            account_table.modify( *itr, same_payer, [&]( auto& a ) {
                a.balance.amount = final_balance;
            });
            if (final_balance == 0) {
                // we call close to release RAM
                action(
                    permission_level{owner,"active"_n},
                    get_self(),
                    "close"_n,
                    std::make_tuple(owner, value.symbol)
                ).send();
            }

        }

        void find_app_deposit(name user, uint64_t app, container_asset &asset, container_instance &container) {
            // find container_spec for app|deposit
            slug appnick = vapaee::bgbox::get_author_nick(app);
            slug asset_slug = slug(appnick.to_string() + ".deposit");
            get_container_for_slug(user, asset_slug, asset, container);
        }

        void find_app_inventory(name user, uint64_t app, container_asset &asset, container_instance &container) {
            // find container_spec for app|inventory
            slug appnick = vapaee::bgbox::get_author_nick(app);
            slug asset_slug = slug(appnick.to_string() + ".inventory");
            get_container_for_slug(user, asset_slug, asset, container);
        }

        void get_container_for_slug(name user, slug asset_slug, container_asset &asset, container_instance &container) {
            get_container_asset_for_slug(asset_slug, asset);
            
            // find container_instance of the user for that spec
            containers cont_table(get_self(), user.value);
            auto index_inv = cont_table.template get_index<"asset"_n>();
            auto itr_inv = index_inv.find( asset.id );
            eosio_assert(itr_inv != index_inv.end(), "no container registered for app");

            container.id = itr_inv->id;
            container.asset = itr_inv->asset;
            container.empty = itr_inv->empty;
            container.space = itr_inv->space;
            container.slots = itr_inv->slots;
        }        
        
    }; // class

}; // namespace

}; // namespace