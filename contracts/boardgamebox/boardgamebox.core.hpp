#pragma once
#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <vapaee/slug_asset.hpp>
#include <vapaee/datatypes.hpp>
#include <vapaee/vapaee_aux_functions.hpp>


#include "boardgamebox.tables.hpp"

using namespace eosio;
namespace vapaee {
    namespace bgbox {

    class core {
        name _self;

    public:
        
        core():_self("boardgamebox"_n){}

        inline name get_self()const { return _self; }

        // APPS ---------------------------

        // -- INCOMPLETO --
        void action_new_app(name author_owner, uint64_t author_app, int invespace) {
            action(
                permission_level{author_owner,"active"_n},
                get_self(),
                "newcontainer"_n,
                std::make_tuple(author_owner, author_app, "inventory", invespace)
            ).send();

            action(
                permission_level{author_owner,"active"_n},
                get_self(),
                "newcontainer"_n,
                std::make_tuple(author_owner, author_app, "deposit", 0)
            ).send();

            slug app_slug = vapaee::get_author_nick(author_app);
            container_specs container_table(get_self(), get_self().value);
            auto index = container_table.template get_index<"second"_n>();

            // inventory
            auto inv_itr = index.find(vapaee::combine(author_app, "inventory"_n));
            slug inv_slug = slug(app_slug.to_string() + ".inventory");
            action(
                permission_level{author_owner,"active"_n},
                get_self(),
                "newinventory"_n,
                std::make_tuple(author_owner, author_app, inv_slug, invespace, inv_itr->id)
            ).send();
            // deposit
            auto dep_itr = index.find(vapaee::combine(author_app, "deposit"_n));
            slug dep_slug = slug(app_slug.to_string() + ".deposit");
            action(
                permission_level{author_owner,"active"_n},
                get_self(),
                "newinventory"_n,
                std::make_tuple(author_owner, author_app, dep_slug, invespace, dep_itr->id)
            ).send();            

        }

        void action_new_item_spec(name author_owner, uint64_t author_app, name nickname, int maxgroup) {
            // exigir al firma de author_owner y que sea el owner del author
            name app_contract = vapaee::get_owner_for_author(author_app);
            require_auth(app_contract);
            eosio_assert(app_contract == author_owner, "given author_owner is not the current owner of author");
            
            // verifica que no exista un nickname para el author_app
            item_specs item_table(get_self(), get_self().value);
            auto index = item_table.template get_index<"second"_n>();
            auto itr = index.find(vapaee::combine(author_app, nickname));
            eosio_assert(itr == index.end(), (nickname.to_string()  + " already registered as item").c_str());
            // crea un nuevo item_spec 
            item_table.emplace( author_owner, [&]( auto& s ) {
                s.id = item_table.available_primary_key();
                s.nick = nickname;
                s.app = author_app;
                s.maxgroup = maxgroup;
            });
        };

        void action_new_container_spec(name author_owner, uint64_t author_app, name nickname, int space) {
            // exigir al firma de author_owner y que sea el owner del author
            name app_contract = vapaee::get_owner_for_author(author_app);
            require_auth(app_contract);
            eosio_assert(app_contract == author_owner, "given author_owner is not the current owner of author");

            // se fija que no exista en container_spec un nickname para author_app 
            container_specs container_table(get_self(), get_self().value);
            auto index = container_table.template get_index<"second"_n>();
            auto itr = index.find(vapaee::combine(author_app, nickname));
            eosio_assert(itr == index.end(), (nickname.to_string()  + " already registered as container").c_str());
            // crea en container_spec un row con nickname para author_app
            container_table.emplace( author_owner, [&]( auto& s ) {
                s.id = container_table.available_primary_key();
                s.nick = nickname;
                s.app = author_app;
            });            
        }

        // PUBLISHERS ---------------------------
        void action_new_item_asset(name author_owner, uint64_t author_issuer, const slug_asset &maximum_supply, uint64_t spec) {
            // una entrada en item_asset con supply 0
            // verifica que no exista un nickname para el author_app
            item_assets assets_table(get_self(), get_self().value);
            auto index = assets_table.template get_index<"slug"_n>();
            auto itr = index.find(maximum_supply.symbol.code().raw().to128bits());
            eosio_assert(itr == index.end(), (maximum_supply.to_string()  + " already registered as asset").c_str());

            // TODO: tengo que exigir que el maximum_supply.symbol.code().raw() tenga com prefijo al nick del author

            // crea un nuevo item_spec 
            assets_table.emplace( author_owner, [&]( auto& s ) {
                s.id = assets_table.available_primary_key();
                s.supply.amount = 0;
                s.supply.symbol = maximum_supply.symbol;
                s.max_supply = maximum_supply;
                s.spec = spec; // table item_spec
                s.publisher = author_issuer; // table vapaeeaouthor::authors.id
                s.block = current_time();
            });            
        }
        
        void action_new_container_asset(name author_owner, uint64_t author_publisher, slug container_slug, int space, uint64_t spec) {
            // una entrada en item_asset con supply 0
            // verifica que no exista un nickname para el author_app
            container_assets cont_assets_table(get_self(), get_self().value);
            auto index = cont_assets_table.template get_index<"slug"_n>();
            auto itr = index.find(container_slug.to128bits());
            eosio_assert(itr == index.end(), (container_slug.to_string()  + " already registered as container asset").c_str());

            // TODO: tengo que exigir que el container_slug tenga com prefijo al nick del author
            container_specs cont_specs_table(get_self(), get_self().value);
            index.get(spec, "container_spec not found");

            // crea un nuevo item_spec 
            cont_assets_table.emplace( author_owner, [&]( auto& s ) {
                s.id = cont_assets_table.available_primary_key();
                s.supply.amount = 0;
                s.supply.symbol = slug_symbol{container_slug};
                s.spec = spec; // table conainer_spec
                s.publisher = author_publisher; // table vapaeeaouthor::authors.id
                s.block = current_time();
            });
        }
        
        void action_issue_item_units(name to, slug_asset quantity, string memo) {
            slug       asset_slug = quantity.symbol.code().raw();
            item_asset asset;
            item_spec  spec;
            item_unit units;
            get_item_spec_for_slug(asset_slug, asset, spec);

            eosio_assert( quantity.is_valid(), "invalid quantity" );
            eosio_assert( quantity.amount > 0, "must issue positive quantity" );            

            // tiene que requerir la firma de quien sea que sea el owner actual del publisher de este asset
            name owner = vapaee::get_owner_for_author(asset.publisher);
            require_auth(owner);

            // update total supply
            item_assets assets_table(get_self(), get_self().value);
            eosio_assert(quantity.amount <= asset.max_supply.amount - asset.supply.amount, "Can't print over maximun");
            assets_table.modify( asset, same_payer, [&]( auto& s ) {
                s.supply.amount += quantity.amount;
            });

            auto ram_payer = has_auth( to ) ? to : owner;
            add_balance(to, quantity, ram_payer);

            // cargo un item_unit con los datos del issue
            units.owner = to;
            units.asset = asset.id;
            units.quantity = quantity.amount;
            // hago instanciar en memoria un item_unit con estos datos en un slot del container "deposit" de este usuario
            allocate_slot_in_deposit(to, quantity.symbol, spec, units);            
        };

        // USERS ---------------------------
        void action_transfer_item_unit_quantity(
            name         from,
            name         to,
            slug_asset   quantity,
            string       memo) {

            vector<uint64_t> units;
            vector<int> quantities;
            collect_units_for_asset(from, quantity, units, quantities);

            // we call for a unit list trastaction to be perfpormed
            action(
                permission_level{from,"active"_n},
                get_self(),
                "transfunits"_n,
                std::make_tuple(from, to, quantity, units, quantities, memo)
            ).send();                
        }

        void action_transfer_item_unit_list(
            name         from,
            name         to,
            slug_asset   quantity,
            const vector<uint64_t> items,
            vector<int> quantities,
            string       memo ) {

            // tiene que haber coherencia entre las listas
            eosio_assert( items.size() == quantities.size(), "items list and quantities list mus match sizes" );
            
            // tiene que exigir la firma de "from", que no sea autopago y que exista el destinatario
            eosio_assert( from != to, "cannot transfer to self" );
            require_auth( from );
            eosio_assert( is_account( to ), "to account does not exist");

            // params validation
            eosio_assert( quantity.is_valid(), "invalid quantity" );
            eosio_assert( quantity.amount > 0, "must transfer positive quantity" );
            eosio_assert( memo.size() <= 256, "memo has more than 256 bytes" );

            // tiene que descontar del balance de "from" y aumentarlo en "to" (table account)
            auto ram_payer = has_auth( to ) ? to : from;
            sub_balance(from, quantity);
            add_balance(to, quantity, ram_payer);

            // Encuentro un nuevo slot vacío para cada unidad y a continuación hago un swap
            item_unit unit;
            item_asset asset;
            item_spec spec;
            get_item_spec_for_slug(quantity.symbol.code().raw(), asset, spec);
            for (int i=0; i<items.size(); i++) {
                find_inventory_slot(to, quantity.symbol, spec, unit);
                action_swap_slots_of_item_unit(from, items[i], spec.app, quantities[i], to, unit.slot, ram_payer);
            }                
        }
        
        void action_new_container_instance (name user, slug container_slug, name ram_payer) {
            require_auth(user);

            container_asset asset;
            container_spec spec;
            get_container_spec_for_slug(container_slug, asset, spec);
            
            containers user_containers(get_self(), user.value);
            auto reg_cont_index = user_containers.template get_index<"asset"_n>();
            auto user_cont_itr = reg_cont_index.find(asset.id);
            eosio_assert(user_cont_itr == reg_cont_index.end(), "container already exist for this user");

            user_containers.emplace(ram_payer, [&](auto&s) {
                s.id = user_containers.available_primary_key();
                s.asset = asset.id;
                s.space = asset.space;
                s.empty = asset.space;
            });
        }

        void action_create_containers_for_user (name user, uint64_t app, name ram_payer) {
            // El objetivo es crearle al usuario todas las instancias de containers y masteries registrados por esa app
            require_auth(user);

            container_assets registered_cont(get_self(), get_self().value);
            auto reg_cont_index = registered_cont.template get_index<"publisher"_n>();

            containers user_containers(get_self(), user.value);
            auto cont_index = user_containers.template get_index<"asset"_n>();
            // auto user_cont_itr = cont_index.begin();
            // iterar sobre todos los container_spec registrados por esta app
            for (
                auto reg_cont_itr = reg_cont_index.lower_bound(app);
                reg_cont_itr != reg_cont_index.end() && reg_cont_itr->publisher == app;
                reg_cont_itr++ ) {

                auto user_cont_itr = cont_index.find(reg_cont_itr->id);
                if (user_cont_itr == cont_index.end()) {
                    // we call for a unit list trastaction to be perfpormed
                    action(
                        permission_level{user,"active"_n},
                        get_self(),
                        "newusercont"_n,
                        std::make_tuple(user, reg_cont_itr->supply.symbol.code().raw(), ram_payer)
                    ).send();
                }
            }
        };

        void action_swap_slots_of_item_unit(
            name from,
            uint64_t unit,
            uint64_t app,
            int quantity,
            name to,
            const slotinfo &target_slot,
            name ram_payer) {

            // assert positive quantity
            eosio_assert(quantity > 0, "can't transfer 0 units");

            // SWAP: un usuario ueve de lugar uno de sus items
            // no se crean ni se destruyen rows, sólo se modifican
            // hay que ver si existe algo en el target_slot
            item_units units_table(get_self(), app);
            auto slots_index = units_table.template get_index<"slot"_n>();
            auto target_slot_itr = slots_index.find(target_slot.to128bits());
            eosio_assert(target_slot_itr->id != unit, "swap orign and destiny slots can't be the same");
            
            // origin_slot
            auto from_units_itr = units_table.find(unit);
            string error_str1 = string("unit does not exist for account ") + from.to_string();
            eosio_assert(from_units_itr != units_table.end(), error_str1.c_str());
            eosio_assert(quantity <= from_units_itr->quantity, "cant transfer more units that exist in origin slot");
            eosio_assert(from_units_itr->owner == from, "origin slot owner != 'from'");                

            // asset table
            item_assets assets_table(get_self(), get_self().value);
            auto from_asset_itr = assets_table.find(from_units_itr->asset);
            eosio_assert(from_asset_itr != assets_table.end(), "assset 'from' does no exist");

            // item spec table
            item_specs specs_table(get_self(), get_self().value);
            auto from_spec_itr = specs_table.find(from_asset_itr->spec);
            eosio_assert(from_spec_itr != specs_table.end(), "item spec 'from' does no exist");

            // from container table
            containers from_containers_table(get_self(), from.value);
            auto from_cont_itr = from_containers_table.find(from_units_itr->slot.container);
            eosio_assert(from_cont_itr != from_containers_table.end(), "contaner instance for 'from' does no exist");
            
            // to container table
            containers to_containers_table(get_self(), to.value);
            auto to_cont_itr = to_containers_table.find(target_slot.container);
            eosio_assert(to_cont_itr != to_containers_table.end(), "contaner instance for 'to' does no exist");                
            // TODO: es posible que el destinatario desee que se le cree en ese momento el inventario

            if (target_slot_itr != slots_index.end() && target_slot_itr->quantity > 0) {
                // hay algo en el slot destino así que debemos hacer un swap o  acumular las unidades en el target_slot
                // hay que averiguar si se pueden acumular en el mismo slot 
                bool acumulables = false;

                eosio_assert(target_slot_itr->owner == to, "target slot owner != 'to'");                

                // hay que averiguar si ambos unidades pertenecen al mismo item_asset
                auto to_asset_itr = assets_table.find(target_slot_itr->asset);
                eosio_assert(to_asset_itr != assets_table.end(), "assset 'to' does no exist");
                
                auto to_spec_itr = specs_table.find(to_asset_itr->spec);
                eosio_assert(to_spec_itr != specs_table.end(), "item spec 'to' does no exist");
                
                // pertenecen al mismo tipo de item?
                if (to_spec_itr == from_spec_itr) {
                    // si es así, hay que preguntar si el item_spec especifica un máximo para el agrupamiento (dif de 1)
                    // si el maxgroup == 0 -> son acumulables
                    if (to_spec_itr->maxgroup == 0) {
                        acumulables = true;
                    }
                    // si el maxgroup > 1 ?
                    else if (to_spec_itr->maxgroup > 1) {
                        eosio_assert(target_slot_itr->quantity + quantity <= to_spec_itr->maxgroup, "exiding maxgroup in target slot");
                        acumulables = true;
                    }
                }

                if (acumulables) {
                    // hay que incrementarle el target_slot_itr->quantity
                    units_table.modify( *target_slot_itr, same_payer, [&]( auto& a ) {
                        a.quantity += quantity;
                    });

                    if (quantity == from_units_itr->quantity) {
                        // Si el contenedor origen es real, tengo que incrementar en 1 el empty porque voy a eliminar un slot y juntar todo en el target_slot
                        from_containers_table.modify(*from_cont_itr, ram_payer, [&](auto &a){
                            a.empty += 1;
                        });

                        // this slot is empty
                        units_table.modify( *from_units_itr, ram_payer, [&]( auto& a ) {
                            a.quantity = 0;
                        });
                    } else {
                        // hay que restarle al slot origen quantity
                        units_table.modify( *from_units_itr, ram_payer, [&]( auto& a ) {
                            a.quantity -= quantity;
                        });
                    }
                } else {
                    // el dato empty de los containers permanece intacto (se intercambia slot x slot)
                    // deben modificarse ambas entradas, intercambiando la infoslot en cada una
                    units_table.modify( *target_slot_itr, ram_payer, [&]( auto& a ) {
                        a.slot.container = from_units_itr->slot.container;
                        a.slot.position = from_units_itr->slot.position;
                        a.owner = from;
                    });

                    units_table.modify( *from_units_itr, same_payer, [&]( auto& a ) {
                        a.slot.container = target_slot.container;
                        a.slot.position = target_slot.position;
                        a.owner = to;
                    });
                }                    
            } else {
                // no existe el item_unit para el target_slot dado o existe pero está vacío (con quantity = 0)
                // vemos si hay que mover todas las unidades del slot
                if (quantity == from_units_itr->quantity) {
                    if (target_slot_itr != slots_index.end()) {
                        // en este caso ya existía un slot instanciado en ese lugar destino, así que se lo intercambiamos al 'from' con el lugar de origen
                        eosio_assert(target_slot_itr->quantity == 0, "swap target slot is not empty");
                        units_table.modify( *target_slot_itr, ram_payer, [&]( auto& a ) {
                            a.slot.container = from_units_itr->slot.container;
                            a.slot.position = from_units_itr->slot.position;
                            a.owner = from;
                        });
                    }
                    // el 'from' simplemente le pasa el slot entero a 'to'
                    units_table.modify( *from_units_itr, same_payer, [&]( auto& a ) {
                        a.slot.container = target_slot.container;
                        a.slot.position = target_slot.position;
                        a.owner = to;
                    });

                    if (to_cont_itr != from_cont_itr) {
                        // decrementar empty destino
                        to_containers_table.modify(*to_cont_itr, same_payer, [&](auto &a){
                            a.empty -= 1;
                        }); 

                        // incrementar empty origen
                        from_containers_table.modify(*from_cont_itr, ram_payer, [&](auto &a){
                            a.empty += 1;
                        });
                    }
                } else {
                    // transfer parcial
                    // hay que restarle al slot origen quantity unidades
                    units_table.modify( *from_units_itr, ram_payer, [&]( auto& a ) {
                        a.quantity -= quantity;
                    });
                    
                    if (target_slot_itr != slots_index.end()) {
                        // hay que crear una instancia nueva de item_units con el quantity
                        units_table.emplace(ram_payer, [&](auto &a){
                            a.id = units_table.available_primary_key();
                            a.owner = to;
                            a.asset = from_units_itr->asset;
                            a.slot.container = target_slot.container;
                            a.slot.position = target_slot.position;
                            a.quantity = quantity;
                        });
                    } else {
                        // ya tenemos el slot, hay que cargarle las unidades
                        eosio_assert(target_slot_itr->quantity == 0, "swap target slot is not empty");
                        units_table.modify( *target_slot_itr, same_payer, [&]( auto& a ) {
                            a.asset = from_units_itr->asset;
                            a.quantity = quantity;
                        });                            
                    }
                    // hay que asegurar que existe el target_slot.container
                    eosio_assert(to_cont_itr !=  to_containers_table.end(), "target container does not exist");

                    // decrementar empty destino
                    to_containers_table.modify(*to_cont_itr, same_payer, [&](auto &a){
                        a.empty -= 1;
                    });
                }
            }                

        };

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
            slug appnick = vapaee::get_author_nick(app);
            slug asset_slug = slug(appnick.to_string() + ".deposit");
            get_container_for_slug(user, asset_slug, asset, container);
        }

        void find_app_inventory(name user, uint64_t app, container_asset &asset, container_instance &container) {
            // find container_spec for app|inventory
            slug appnick = vapaee::get_author_nick(app);
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