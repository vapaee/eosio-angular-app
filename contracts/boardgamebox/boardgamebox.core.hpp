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

        void action_new_item_spec(name owner, uint64_t app_author_id, name nickname, int maxgroup) {
            print("vapaee::bgbox::core::action_new_item_spec()\n");
            print(" owner: ", owner.to_string(), "\n");
            print(" app_author_id: ", std::to_string((int) app_author_id), "\n");
            print(" nickname: ", nickname.to_string(), "\n");
            print(" maxgroup: ", std::to_string((int) maxgroup), "\n");            
            
            // owner must be the app owner and sign
            name app_owner = vapaee::bgbox::get_author_owner(app_author_id);
            require_auth(app_owner);
            eosio_assert(app_owner == owner, "given owner is not the current owner of author");
            
            // verifies item-nickname is not registered for app
            item_specs item_table(get_self(), get_self().value);
            auto index = item_table.template get_index<"second"_n>();
            auto itr = index.find(vapaee::utils::combine(app_author_id, nickname));
            eosio_assert(itr == index.end(), (nickname.to_string()  + " already registered as item").c_str());

            // crea un nuevo item_spec 
            item_table.emplace( owner, [&]( auto& s ) {
                s.id = item_table.available_primary_key();
                s.nick = nickname;
                s.app = app_author_id;
                s.maxgroup = maxgroup;
            });

            print(" item_specs.emplace() new item spec ", nickname, " regitered\n" );
            
            print("vapaee::bgbox::core::action_new_item_spec() ends...\n");
        };
        
        void action_new_container_spec(name owner, uint64_t app_author_id, name nickname) {
            print("vapaee::bgbox::core::action_new_container_spec()\n");
            print(" owner: ", owner.to_string(), "\n");
            print(" app_author_id: ", std::to_string((int) app_author_id), "\n");
            print(" nickname: ", nickname.to_string(), "\n");

            // owner must be the app owner and sign
            name app_owner = vapaee::bgbox::get_author_owner(app_author_id);
            require_auth(app_owner);
            eosio_assert(app_owner == owner, "given owner is not the current owner of author");

            // get the next container spec id
            container_specs container_table(get_self(), get_self().value);
            uint64_t container_id = container_table.available_primary_key();
            print(" container_id: ", std::to_string((int) container_id), "\n");

            // verifies container-nickname is not registered for app
            auto index = container_table.template get_index<"second"_n>();
            auto itr = index.find(vapaee::utils::combine(app_author_id, nickname));
            eosio_assert(itr == index.end(), (nickname.to_string()  + " already registered as container with id " + std::to_string((int)itr->id)).c_str());

            // create a new container_spec for app
            container_table.emplace( owner, [&]( auto& s ) {
                s.id = container_id;
                s.nick = nickname;
                s.app = app_author_id;
            });

            print(" container_specs.emplace() new container spec ", nickname, " regitered\n" );
            
            print("vapaee::bgbox::core::action_new_container_spec() ends...\n");
            
        }

        // PUBLISHERS ---------------------------

        void action_new_item_asset(name author_owner, uint64_t author_issuer, const slug_asset &maximum_supply, uint64_t spec) {
            print("vapaee::bgbox::core::action_new_item_asset()\n");
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

            print(" item_assets.emplace() new item asset ", maximum_supply.to_string(), " regitered\n" );
            
            print("vapaee::bgbox::core::action_new_item_asset() ends...\n");
        }

        void action_new_container_asset(name author_owner, uint64_t author_publisher, slug container_slug, int space, uint64_t spec) {
            print("vapaee::bgbox::core::action_new_container_asset()\n");
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
                s.space = space;
            });
            print(" container_assets.emplace() new container assets ", container_slug.to_string(), " regitered\n" );

            print("vapaee::bgbox::core::action_new_container_asset() ends...\n");
        }

        void action_issue_item_units(uint64_t to, slug_asset quantity, const string &memo) {
            print("vapaee::bgbox::core::action_issue_item_units()\n");
            print(" to: ", std::to_string((int) to), "\n");
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

            add_balance(to, quantity, owner);

            // cargo un item_unit con los datos del issue
            units.owner = to;
            units.asset = asset.id;
            units.quantity = quantity.amount;
            // hago instanciar en memoria un item_unit con estos datos en un slot del container "deposit" de este usuario
            vapaee::bgbox::allocate_slot_in_deposit(to, quantity.symbol, spec, units);            
            
            print("vapaee::bgbox::core::action_issue_item_units() ends...\n");
        };

        // USERS ---------------------------
        void action_transfer_item_unit_quantity(
            name          owner,
            uint64_t      from,
            uint64_t      to,
            slug_asset    quantity,
            const string& memo) {
            
            print("vapaee::bgbox::core::action_transfer_item_unit_quantity()\n");
            print(" owner: ", owner.to_string(), "\n");
            print(" from: ", std::to_string((int)from), "\n");
            print(" to: ", std::to_string((int)to), "\n");
            print(" quantity: ", quantity.to_string(), "\n");
            print(" memo: ", memo, "\n");

            vector<uint64_t> units;
            vector<int> quantities;
            vapaee::bgbox::collect_units_for_asset(from, quantity, units, quantities);

            // we call for a unit list trastaction to be perfpormed
            action(
                permission_level{owner,"active"_n},
                get_self(),
                "transfunits"_n,
                std::make_tuple(owner, from, to, quantity, units, quantities, memo)
            ).send();
            

            print("vapaee::bgbox::core::action_transfer_item_unit_quantity() ends...\n");
        }

        void action_transfer_item_unit_list(
            name          owner,
            uint64_t      from,
            uint64_t      to,
            slug_asset   quantity,
            const vector<uint64_t> items,
            vector<int> quantities,
            const string& memo) {

            print("vapaee::bgbox::core::action_transfer_item_unit_list()\n");
            print(" owner: ", owner.to_string(), "\n");
            print(" from: ", std::to_string((int) from), "\n");
            print(" to: ", std::to_string((int) to), "\n");
            print(" items.size(): ", std::to_string((int) items.size()), "\n");
            print(" quantity: ", quantity.to_string(), "\n");
            print(" memo: ", memo, "\n");

            // tiene que haber coherencia entre las listas
            eosio_assert( items.size() == quantities.size(), "items list and quantities list must match sizes" );
            
            // tiene que exigir la firma de "from", que no sea autopago y que exista el destinatario
            name from_owner = vapaee::bgbox::get_author_owner(from);
            name to_owner = vapaee::bgbox::get_author_owner(to);

            string err1 = string("owner (") + owner.to_string() + ") is not from-profile's owner (" + from_owner.to_string() + ")";
            eosio_assert( from_owner == owner, err1.c_str() );
            eosio_assert( from != to, "cannot transfer to self" );
            require_auth( owner );
            string err2 = string("to-tpfiles's owner (") + to_owner.to_string() + ") account does not exist";
            eosio_assert( is_account( to_owner ), err2.c_str());

            // params validation
            eosio_assert( quantity.is_valid(), "invalid quantity" );
            eosio_assert( quantity.amount > 0, "must transfer positive quantity" );
            eosio_assert( memo.size() <= 256, "memo has more than 256 bytes" );

            // tiene que descontar del balance de "from" y aumentarlo en "to" (table account)
            auto ram_payer = has_auth( to_owner ) ? to_owner : from_owner;
            vapaee::bgbox::sub_balance(from, quantity);
            vapaee::bgbox::add_balance(to, quantity, ram_payer);

            // Encuentro un nuevo slot vacío para cada unidad y a continuación hago un swap
            item_unit unit;
            item_asset asset;
            item_spec spec;
            vapaee::bgbox::get_item_spec_for_slug(quantity.symbol.code().raw(), asset, spec);
            for (int i=0; i<items.size(); i++) {
                vapaee::bgbox::find_inventory_slot(to, quantity.symbol, spec, unit);
                action_swap_slots_of_item_unit(from_owner, from, items[i], spec.app, quantities[i], to, unit.slot, ram_payer);
            }
            
            print("vapaee::bgbox::core::action_transfer_item_unit_list() ends...\n");
        }

        void action_new_container_instance (name owner, uint64_t profile, std::string container_slug_str, name ram_payer) {
            print("vapaee::bgbox::core::action_new_container_instance()\n");
            print(" owner: ", owner.to_string(), "\n");
            print(" profile: ", std::to_string((int)profile), "\n");
            print(" container_slug_str: ", container_slug_str.c_str(), "\n");
            print(" ram_payer: ", ram_payer.to_string(), "\n");
            require_auth(owner);

            name profile_owner = vapaee::bgbox::get_author_owner(profile);
            eosio_assert(profile_owner == owner, (owner.to_string() + " is not the owner of profile " + std::to_string((int)profile)).c_str());

            slug container_slug(container_slug_str);
            print(" container_slug: ", container_slug.to_string(), "\n");

            container_asset asset;
            container_spec spec;
            vapaee::bgbox::get_container_spec_for_slug(container_slug, asset, spec);
            print(" asset.space: ", std::to_string((int) asset.space), "\n");
            
            containers user_containers(get_self(), profile);
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

            print("vapaee::bgbox::core::action_new_container_instance() ends...\n");
        }

        void action_create_containers_for_profile (name owner, uint64_t profile, uint64_t app, name ram_payer) {
            print("vapaee::bgbox::core::action_create_containers_for_profile()\n");
            print(" owner: ", owner.to_string(), "\n");
            print(" profile: ", std::to_string((int) profile), "\n");
            print(" app: ", std::to_string((int) app), "\n");
            print(" ram_payer: ", ram_payer.to_string(), "\n");

            /*
            El usuario acaba de llegar a la app y lo mandaron para acá a inicializarse. Lo que la app necesitea es que el usuario
            tenga un contededor correspondiente a cada uno de los container_assets
            que están definidos para la app (previamente por la propia app)

            Ahora que lo pienso esto es muy agresivo porque la app podría tener muchos contenedores definidos
            */


            // El objetivo es crearle al usuario todas las instancias de containers y masteries registrados por esa app
            require_auth(owner);

            name profile_owner = vapaee::bgbox::get_author_owner(profile);
            string error_str = string("ERROR: owner '") + owner.to_string() + "' is not the current owner of profile " + std::to_string((int) profile);
            eosio_assert(profile_owner == owner, error_str.c_str());

            container_assets registered_cont(get_self(), get_self().value);
            auto reg_cont_index = registered_cont.template get_index<"publisher"_n>();

            containers user_containers(get_self(), profile);
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
                    print(" creating container '",reg_cont_itr->supply.symbol.code().raw().to_string(),"' for owner '", owner.to_string(), "' \n");

                    action(
                        permission_level{owner,"active"_n},
                        get_self(),
                        "cont4profile"_n,
                        std::make_tuple(owner, profile, reg_cont_itr->supply.symbol.code().raw().to_string(), ram_payer)
                    ).send();
                }
            }
            print("vapaee::bgbox::core::action_create_containers_for_profile() ends...\n");
        };

        void action_swap_slots_of_item_unit(
            name from_owner,
            uint64_t from_profile,
            uint64_t unit,
            uint64_t app,
            int quantity,
            uint64_t to_profile,
            const slotinfo &target_slot,
            name ram_payer) {

            print("vapaee::bgbox::core::action_swap_slots_of_item_unit()\n");
            print(" from_owner: ", from_owner.to_string(), "\n");
            print(" from_profile: ", std::to_string((int) from_profile), "\n");
            print(" unit: ", std::to_string((int) unit), "\n");
            print(" app: ", std::to_string((int) app), "\n");
            print(" quantity: ", std::to_string((int) quantity), "\n");
            print(" to_profile: ", std::to_string((int) to_profile), "\n");
            print(" target_slot: ", target_slot.to_string(), "\n");
            print(" ram_payer: ", ram_payer.to_string(), "\n");

            name from = vapaee::bgbox::get_author_owner(from_profile);
            name to = vapaee::bgbox::get_author_owner(to_profile);
            print(" from: ", from.to_string(), "\n");
            print(" to: ", to.to_string(), "\n");

            // assert positive quantity
            eosio_assert(quantity > 0, "can't transfer 0 units");

            // SWAP: un usuario mueve de lugar uno de sus items
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
            eosio_assert(from_units_itr->owner == from_profile, "origin slot owner != 'from'");                

            // asset table
            item_assets assets_table(get_self(), get_self().value);
            auto from_asset_itr = assets_table.find(from_units_itr->asset);
            eosio_assert(from_asset_itr != assets_table.end(), "assset 'from' does no exist");

            // item spec table
            item_specs specs_table(get_self(), get_self().value);
            auto from_spec_itr = specs_table.find(from_asset_itr->spec);
            eosio_assert(from_spec_itr != specs_table.end(), "item spec 'from' does no exist");

            // from container table
            containers from_containers_table(get_self(), from_profile);
            auto from_cont_itr = from_containers_table.find(from_units_itr->slot.container);
            eosio_assert(from_cont_itr != from_containers_table.end(), "contaner instance for 'from' does no exist");
            
            // to container table
            containers to_containers_table(get_self(), to_profile);
            auto to_cont_itr = to_containers_table.find(target_slot.container);
            eosio_assert(to_cont_itr != to_containers_table.end(), "contaner instance for 'to' does no exist");                
            // TODO: es posible que el destinatario desee que se le cree en ese momento el inventario

            print(" - root\n");

            if (target_slot_itr != slots_index.end() && target_slot_itr->quantity > 0) {
                print(" - root/target-not-empty\n");
                // hay algo en el slot destino así que debemos hacer un swap o  acumular las unidades en el target_slot
                // hay que averiguar si se pueden acumular en el mismo slot 
                bool acumulables = false;

                eosio_assert(target_slot_itr->owner == to_profile, "target slot owner != 'to'");                

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
                        a.owner = from_profile;
                    });

                    units_table.modify( *from_units_itr, same_payer, [&]( auto& a ) {
                        a.slot.container = target_slot.container;
                        a.slot.position = target_slot.position;
                        a.owner = to_profile;
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
                            a.owner = from_profile;
                        });
                    }
                    // el 'from' simplemente le pasa el slot entero a 'to'
                    units_table.modify( *from_units_itr, same_payer, [&]( auto& a ) {
                        a.slot.container = target_slot.container;
                        a.slot.position = target_slot.position;
                        a.owner = to_profile;
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
                            a.owner = to_profile;
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
            
            print("vapaee::bgbox::core::action_swap_slots_of_item_unit() ends...\n");
        };
        
    }; // class

}; // namespace

}; // namespace