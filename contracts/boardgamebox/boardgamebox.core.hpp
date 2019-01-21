#pragma once
#include <vapaee/bgbox/common.hpp>


using namespace eosio;
namespace vapaee {
    namespace bgbox {

    class core {
        name _self;
    
    public:
        
        core():_self("boardgamebox"_n){}

        inline name get_self()const { return _self; }

        // APPS ---------------------------

        void action_new_item_spec(name author_owner, uint64_t author_app, name nickname, int maxgroup) {
            print("core::action_new_container_spec()\n");
            print(" author_owner: ", author_owner.to_string(), "\n");
            print(" author_app: ", std::to_string((int) author_app), "\n");
            print(" nickname: ", nickname.to_string(), "\n");
            print(" maxgroup: ", std::to_string((int) maxgroup), "\n");            
            
            // exigir al firma de author_owner y que sea el owner del author
            name app_owner = vapaee::bgbox::get_author_owner(author_app);
            require_auth(app_owner);
            eosio_assert(app_owner == author_owner, "given author_owner is not the current owner of author");
            
            // verifica que no exista un nickname para el author_app
            item_specs item_table(get_self(), get_self().value);
            auto index = item_table.template get_index<"second"_n>();
            auto itr = index.find(vapaee::bgbox::combine(author_app, nickname));
            eosio_assert(itr == index.end(), (nickname.to_string()  + " already registered as item").c_str());
            // crea un nuevo item_spec 
            item_table.emplace( author_owner, [&]( auto& s ) {
                s.id = item_table.available_primary_key();
                s.nick = nickname;
                s.app = author_app;
                s.maxgroup = maxgroup;
            });
            
            print("core::action_new_container_spec() ends...\n");
        };
        
        void action_new_container_spec(name author_owner, uint64_t author_app, name nickname, int space) {
            print("core::action_new_container_spec()\n");
            print(" author_owner: ", author_owner.to_string(), "\n");
            print(" author_app: ", std::to_string((int) author_app), "\n");
            print(" nickname: ", nickname.to_string(), "\n");
            print(" space: ", std::to_string((int) space), "\n");
            
            // exigir al firma de author_owner y que sea el owner del author
            name app_owner = vapaee::bgbox::get_author_owner(author_app);
            print(" app_owner: ", app_owner.to_string(), "\n");
            eosio_assert(app_owner == author_owner, "given author_owner is not the current owner of author");
            eosio_assert(space >= 0, "inventory space can't be negative");
            // require_auth(app_owner);

            // se fija que no exista en container_spec un nickname para author_app 
            container_specs container_table(get_self(), get_self().value);
            uint64_t container_id = container_table.available_primary_key();
            print(" container_id: ", std::to_string((int) container_id), "\n");

            auto index = container_table.template get_index<"second"_n>();
            auto itr = index.find(vapaee::bgbox::combine(author_app, nickname));
            eosio_assert(itr == index.end(), (nickname.to_string()  + " already registered as container with id " + std::to_string((int)itr->id)).c_str());
            // crea en container_spec un row con nickname para author_app
            container_table.emplace( author_owner, [&]( auto& s ) {
                s.id = container_id;
                s.nick = nickname;
                s.app = author_app;
            });
            
            print("core::action_new_container_spec() ends...\n");
            
        }

        // PUBLISHERS ---------------------------

        void action_new_item_asset(name author_owner, uint64_t author_issuer, const slug_asset &maximum_supply, uint64_t spec) {
            print("core::action_new_item_asset()\n");
            print(" author_owner: ", author_owner.to_string(), "\n");
            print(" author_issuer: ", std::to_string((int) author_issuer), "\n");
            print(" maximum_supply: ", maximum_supply.to_string(), "\n");          
            print(" spec: ", std::to_string((int) spec), "\n");               
            
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
            
            print("core::action_new_item_asset() ends...\n");
        }

        void action_new_container_asset(name author_owner, uint64_t author_publisher, slug container_slug, int space, uint64_t spec) {
            print("core::action_new_container_asset()\n");
            print(" author_owner: ", author_owner.to_string(), "\n");
            print(" author_publisher: ", std::to_string((int) author_publisher), "\n");
            print(" container_slug: ", container_slug.to_string(), "\n");
            print(" space: ", std::to_string((int) space), "\n");            
            print(" spec: ", std::to_string((int) spec), "\n");            
            // una entrada en item_asset con supply 0
            // verifica que no exista un nickname para el author_app
            container_assets cont_assets_table(get_self(), get_self().value);
            auto index = cont_assets_table.template get_index<"slug"_n>();
            auto itr = index.find(container_slug.to128bits());
            eosio_assert(itr == index.end(), (container_slug.to_string()  + " already registered as container asset").c_str());

            // TODO: tengo que exigir que el container_slug tenga com prefijo al nick del author
            container_specs cont_specs_table(get_self(), get_self().value);
            cont_specs_table.get(spec, "container_spec not found");

            // crea un nuevo item_spec 
            cont_assets_table.emplace( author_owner, [&]( auto& s ) {
                s.id = cont_assets_table.available_primary_key();
                s.supply.amount = 0;
                s.supply.symbol = slug_symbol{container_slug};
                s.spec = spec; // table conainer_spec
                s.publisher = author_publisher; // table vapaeeaouthor::authors.id
                s.block = current_time();
            });
            print("core::action_new_container_asset() ends...\n");
        }

        void action_issue_item_units(name to, slug_asset quantity, const string &memo) {
            print("core::action_issue_item_units()\n");
            print(" name: ", to.to_string(), "\n");
            print(" quantity: ", quantity.to_string(), "\n");
            print(" memo: ", memo, "\n");
            
            slug       asset_slug = quantity.symbol.code().raw();
            item_asset asset;
            item_spec  spec;
            item_unit units;
            vapaee::bgbox::get_item_spec_for_slug(asset_slug, asset, spec);

            eosio_assert( quantity.is_valid(), "invalid quantity" );
            eosio_assert( quantity.amount > 0, "must issue positive quantity" );            

            // tiene que requerir la firma de quien sea que sea el owner actual del publisher de este asset
            name owner = vapaee::bgbox::get_author_owner(asset.publisher);
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
            
            print("core::action_issue_item_units() ends...\n");
        };

        // USERS ---------------------------
        void action_transfer_item_unit_quantity(
            name          from,
            name          to,
            slug_asset    quantity,
            const string& memo) {
            
            print("core::action_transfer_item_unit_quantity()\n");
            print(" from: ", from.to_string(), "\n");
            print(" to: ", to.to_string(), "\n");
            print(" quantity: ", quantity.to_string(), "\n");
            print(" memo: ", memo, "\n");

            vector<uint64_t> units;
            vector<int> quantities;
            vapaee::bgbox::collect_units_for_asset(from, quantity, units, quantities);

            // we call for a unit list trastaction to be perfpormed
            action(
                permission_level{from,"active"_n},
                get_self(),
                "transfunits"_n,
                std::make_tuple(from, to, quantity, units, quantities, memo)
            ).send();
            

            print("core::action_transfer_item_unit_quantity() ends...\n");
        }

        void action_transfer_item_unit_list(
            name         from,
            name         to,
            slug_asset   quantity,
            const vector<uint64_t> items,
            vector<int> quantities,
            const string& memo) {

            print("core::action_transfer_item_unit_list()\n");
            print(" name: ", to.to_string(), "\n");
            print(" quantity: ", quantity.to_string(), "\n");
            print(" memo: ", memo, "\n");

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
            vapaee::bgbox::sub_balance(from, quantity);
            vapaee::bgbox::add_balance(to, quantity, ram_payer);

            // Encuentro un nuevo slot vacío para cada unidad y a continuación hago un swap
            item_unit unit;
            item_asset asset;
            item_spec spec;
            vapaee::bgbox::get_item_spec_for_slug(quantity.symbol.code().raw(), asset, spec);
            for (int i=0; i<items.size(); i++) {
                find_inventory_slot(to, quantity.symbol, spec, unit);
                action_swap_slots_of_item_unit(from, items[i], spec.app, quantities[i], to, unit.slot, ram_payer);
            }
            
            print("core::action_transfer_item_unit_list() ends...\n");
        }

        void action_new_container_instance (name user, slug container_slug, name ram_payer) {
            print("core::action_new_container_instance()\n");
            print(" user: ", user.to_string(), "\n");
            print(" container_slug: ", container_slug.to_string(), "\n");
            print(" ram_payer: ", ram_payer.to_string(), "\n");
            require_auth(user);

            container_asset asset;
            container_spec spec;
            vapaee::bgbox::get_container_spec_for_slug(container_slug, asset, spec);
            
            containers user_containers(get_self(), user.value);
            auto reg_cont_index = user_containers.template get_index<"asset"_n>();
            auto user_cont_itr = reg_cont_index.find(asset.id);
            eosio_assert(user_cont_itr == reg_cont_index.end(), "container already exist for this user");

            uint64_t cont_id = user_containers.available_primary_key();
            user_containers.emplace(ram_payer, [&](auto&s) {
                s.id = cont_id;
                s.asset = asset.id;
                s.space = asset.space;
                s.empty = asset.space;
            });

            print(" containers(): ", user_containers.find(cont_id)->to_string(), "\n");

            print("core::action_new_container_instance() ends...\n");
        }

        void action_create_containers_for_user (name user, uint64_t app, name ram_payer) {
            print("core::action_create_containers_for_user()\n");
            print(" user: ", user.to_string(), "\n");
            print(" app: ", std::to_string((int) app), "\n");
            print(" ram_payer: ", ram_payer.to_string(), "\n");
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
                    print(" creating container '",reg_cont_itr->supply.symbol.code().raw().to_string(),"' for user '", user.to_string(), "' \n");

                    action(
                        permission_level{user,"active"_n},
                        get_self(),
                        "newusercont"_n,
                        std::make_tuple(user, reg_cont_itr->supply.symbol.code().raw(), ram_payer)
                    ).send();
                }
            }
            print("core::action_create_containers_for_user() ends...\n");
        };

        void action_swap_slots_of_item_unit(
            name from,
            uint64_t unit,
            uint64_t app,
            int quantity,
            name to,
            const slotinfo &target_slot,
            name ram_payer) {

            print("core::action_swap_slots_of_item_unit()\n");
            print(" from: ", from.to_string(), "\n");
            print(" unit: ", std::to_string((int) unit), "\n");
            print(" app: ", std::to_string((int) app), "\n");
            print(" quantity: ", std::to_string((int) quantity), "\n");
            print(" to: ", to.to_string(), "\n");
            print(" target_slot: ", target_slot.to_string(), "\n");
            print(" ram_payer: ", ram_payer.to_string(), "\n");

            
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

            print(" - root\n");

            if (target_slot_itr != slots_index.end() && target_slot_itr->quantity > 0) {
                print(" - root/target-not-empty\n");
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
                    print(" - root/target-not-empty/same-containers\n");
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
                    print(" - root/target-not-empty/acumulables\n");
                    // hay que incrementarle el target_slot_itr->quantity
                    units_table.modify( *target_slot_itr, same_payer, [&]( auto& a ) {
                        a.quantity += quantity;
                    });

                    if (quantity == from_units_itr->quantity) {
                        print(" - root/target-not-empty/acumulables/normal-transfer\n");
                        // Si el contenedor origen es real, tengo que incrementar en 1 el empty porque voy a eliminar un slot y juntar todo en el target_slot
                        from_containers_table.modify(*from_cont_itr, ram_payer, [&](auto &a){
                            a.empty += 1;
                        });

                        // this slot is empty
                        units_table.modify( *from_units_itr, ram_payer, [&]( auto& a ) {
                            a.quantity = 0;
                        });
                    } else {
                        print(" - root/target-not-empty/acumulables/parcial-transfer\n");
                        // hay que restarle al slot origen quantity
                        units_table.modify( *from_units_itr, ram_payer, [&]( auto& a ) {
                            a.quantity -= quantity;
                        });
                    }
                } else {
                    print(" - root/target-not-empty/not-acumulables\n");
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
                print(" - root/target-empty\n");
                // no existe el item_unit para el target_slot dado o existe pero está vacío (con quantity = 0)
                // vemos si hay que mover todas las unidades del slot
                if (quantity == from_units_itr->quantity) {
                    print(" - root/target-empty/normal-transfer\n");
                    // no se va a modificar el contenido sino el dato de "donde" se ubica el source-slot (y del target-slot si hubiera)
                    // transfer total - se transfieren todas las unidades que hay en source
                    if (target_slot_itr != slots_index.end()) {
                        print(" - root/target-empty/normal-transfer/target-is-instantiated\n");
                        // en este caso ya existía un target-slot instanciado en ese lugar destino, así que se lo intercambiamos al source-slot con el lugar de origen
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
                        print(" - root/target-empty/normal-transfer/different-containers\n");

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
                    print(" - root/target-empty/parcial-transfer\n");
                    // transfer parcial - no todas las unidades se van a transferir
                    // hay que restarle al slot origen quantity unidades
                    units_table.modify( *from_units_itr, ram_payer, [&]( auto& a ) {
                        a.quantity -= quantity;
                    });
                    
                    if (target_slot_itr != slots_index.end()) {
                        print(" - root/target-empty/parcial-transfer/no-target-instantiated\n");
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
                        print(" - root/target-empty/parcial-transfer/target-is-instantiated\n");
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
            
            print("core::action_swap_slots_of_item_unit() ends...\n");
        };
        
    }; // class

}; // namespace

}; // namespace