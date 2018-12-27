#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <vapaee/slug_asset.hpp>
#include <vapaee/datatypes.hpp>
#include <vapaee/vapaee_aux_functions.hpp>

#define inventary_default_space 8

using namespace eosio;
namespace vapaee {

CONTRACT boardbagebox : public eosio::contract {

    private:
        // TABLE account (balances) -----------
            // scope: user
            // row: dice cuantas unidades tiene el usuario de determinado item identificado por un slug
            TABLE account { 
                uint64_t           id;
                uint64_t        asset; // item_asset.id
                slug_asset    balance; // cuantas unidades del item identificado por slug tiene el usuario
                uint64_t primary_key() const { return id;  }
                uint128_t secondary_key() const { return balance.symbol.code().raw().to128bits(); }
            };
            typedef eosio::multi_index<"accounts"_n, account, 
                indexed_by<"second"_n, const_mem_fun<account, uint128_t, &account::secondary_key>>
            > accounts;
        // ------------------------------------

        // TABLE item_unit: donde están las unidades de items
            // scope: user
            // row: representa un slot conteniendo quantity unidades del item definido por asset en la posición position
            TABLE item_unit {
                uint64_t           id;
                uint64_t        asset;  // item_asset
                slotinfo         slot; 
                int          quantity;  // quantity <= item_asset.spec.maxgroup
                uint64_t primary_key() const { return id;  }
                uint64_t asset_key() const { return asset;  }
                uint64_t container_key() const { return slot.container; }
                uint128_t slot_key() const { return slot.to128bits(); }
            };
            typedef eosio::multi_index<"items"_n, item_unit,
                indexed_by<"second"_n, const_mem_fun<item_unit, uint64_t, &item_unit::asset_key>>,
                indexed_by<"container"_n, const_mem_fun<item_unit, uint64_t, &item_unit::container_key>>,
                indexed_by<"slot"_n, const_mem_fun<item_unit, uint128_t, &item_unit::slot_key>>
                
            > item_units;
        // ------------------------------------

        // TABLE container_instance: Los contenedores que tiene un usuario
            // scope: user
            // row: representa un container para ese usuario que indica cuantos lugares libres tienen y que espacio total tiene
            TABLE container_instance {
                uint64_t      id;
                uint64_t    spec; // table container_specs.id
                int        empty;
                int        space;
                uint64_t primary_key() const { return id;  }
                uint64_t spec_key() const { return spec; }
            };
            typedef eosio::multi_index<"containers"_n, container_instance,
                indexed_by<"spec"_n, const_mem_fun<container_instance, uint64_t, &container_instance::spec_key>>
            > containers;
        // ------------------------------------

        // TABLE user_reg: lista de todos los usuarios que usaron el sistema. es para poder iterar sobre todos los nombres
            // scope: contract
            // row: representa un usuario
            TABLE user_reg {
                name user;
                uint64_t primary_key() const { return user.value;  }
            };
            typedef eosio::multi_index<"users"_n, user_reg> users;
        // ------------------------------------

        // TABLE container_spec: Lista de todos los contenedores definidos. Cada app tiene varios contenedores definidos
            // TODAS LAS APPS TIENEN AL MENOS UNA CONTENEDOR LLAMADO INVENTARIO (es una forma de saber si una app está registrada)
            // DEBE DEFINIR UN FILTRO PARA LOS ITEMS QUE SE PUEDEN PONER. dEBE ESPECIFICARSE QUE ITEM_SPEC SE PUEDEN PONER
            // scope: contract
            // row: representa un container definido para la aplicación app con el apodo nick
            TABLE container_spec {
                uint64_t          id;
                name            nick; // el apodo de este container debe ser único para cada app
                uint64_t         app; // table vapaeeaouthor::authors.id
                int            space; // espacio base que tendrá el container cuando se lo instancie para un usuario. 0 para infinito.
                uint64_t primary_key() const { return id;  }
                uint128_t secondary_key() const { return vapaee::combine(app, nick); }
                uint64_t app_key() const { return app; }
            };
            typedef eosio::multi_index<"contspec"_n, container_spec,
                indexed_by<"second"_n, const_mem_fun<container_spec, uint128_t, &container_spec::secondary_key>>,
                indexed_by<"app"_n, const_mem_fun<container_spec, uint64_t, &container_spec::app_key>>
            > container_specs; 
        // ------------------------------------

        // TABLE item_spec: Lista de todos los TIPOS de items que existen. Cada App define varios tipos de items
            // scope: contract
            // row: representa un tipo de item definido por una aplicación app con el nombre nick
            TABLE item_spec { 
                uint64_t          id;
                name            nick; 
                uint64_t         app; // table vapaeeaouthor::authors.id
                int         maxgroup; // MAX (max quantity in same slot), 1 (no agroup), 0 (no limit)
                uint64_t primary_key() const { return id;  }
                uint128_t secondary_key() const { return vapaee::combine(app, nick); }
                uint64_t app_key() const { return app; }
            };
            typedef eosio::multi_index<"itemspec"_n, item_spec,
                indexed_by<"second"_n, const_mem_fun<item_spec, uint128_t, &item_spec::secondary_key>>,
                indexed_by<"app"_n, const_mem_fun<item_spec, uint64_t, &item_spec::app_key>>
            > item_specs;
        // ------------------------------------

        // TABLE item_asset: Lista de todos los ITEMS definidos por alguien (los cuales tienen un TIPO de item)
            // scope: contract
            // row: representa un item, del cual existe una cantidad supply de unidades que no supera el max_supply
            TABLE item_asset {
                uint64_t              id;
                slug_asset        supply;
                slug_asset    max_supply;
                uint64_t            spec; // table item_spec
                uint64_t       publisher; // table vapaeeaouthor::authors.id
                uint64_t           block; 
                uint64_t primary_key() const { return id;  }
                uint128_t secondary_key() const { return supply.symbol.code().raw().to128bits(); }
                uint64_t publisher_key() const { return publisher; }
            };
            typedef eosio::multi_index<"stat"_n, item_asset,
                indexed_by<"second"_n, const_mem_fun<item_asset, uint128_t, &item_asset::secondary_key>>,
                indexed_by<"publisher"_n, const_mem_fun<item_asset, uint64_t, &item_asset::publisher_key>>
            > stats;
        // ------------------------------------

        // TABLE mastery_spec: lista de todas las maestrías definidas por una app
            // scope: contract
            TABLE mastery_spec {
                uint64_t          id;
                name            nick; 
                uint64_t         app; // table vapaeeaouthor::authors.id
                name           table; // table affected: "", "item_spec", "item_asset"
                uint64_t         row; // mastery_prop.id 
                    // "item_spec": esta maestría es para tunear un objeto de tipo descrito en la fila row
                    // "item_asset": esta maestría es para tunear un objeto identificado con un slug item_asset[row].supply.symbol.row()
                uint64_t primary_key() const { return id; }
                uint128_t secondary_key() const { return vapaee::combine(app, nick); }
                uint64_t app_key() const { return app; }
            };
            typedef eosio::multi_index<"mastery"_n, mastery_spec,
                indexed_by<"second"_n, const_mem_fun<mastery_spec, uint128_t, &mastery_spec::secondary_key>>,
                indexed_by<"app"_n, const_mem_fun<mastery_spec, uint64_t, &mastery_spec::app_key>>
            > mastery_specs;
        // ------------------------------------

        // TABLE mastery_prop: lista de properties que tiene una mastery
            // scope: mastery
            // row: representa una property de la maestría
            TABLE mastery_prop {
                uint64_t                   id;
                name                    title; // localstrings.strings.key
                name                     desc; // localstrings.strings.key
                slug                 property;
                iconinfo                 icon;
                std::vector<levelinfo> levels;
                uint64_t primary_key() const { return id;  }
                uint128_t secondary_key() const { return property.to128bits(); }
            };
            typedef eosio::multi_index<"masteryprop"_n, mastery_prop,
                indexed_by<"second"_n, const_mem_fun<mastery_prop, uint128_t, &mastery_prop::secondary_key>>
            > mastery_props;
        // ------------------------------------

        // TABLE experience: Los contenedores que tiene un usuario
            // scope: user
            // row: representa un container para ese usuario que indica cuantos lugares libres tienen y que espacio total tiene
            TABLE experience {
                uint64_t         id;
                uint64_t    mastery; // mastery_spec
                uint64_t  container; // container_instance donde están los Mastery-Tokens
                name          table; // table affected: "", "item_unit", "item_asset"
                uint64_t        row; // row.id
                    // "item_spec": esta maestría es para tunear un objeto de tipo descrito en la fila row
                    // "item_asset": esta maestría es para tunear un objeto identificado con un slug item_asset[row].supply.symbol.row()
                // TODO: ver la posibilidad de implementar un cache aca
                uint64_t primary_key() const { return id; }
                uint64_t secondary_key() const { return mastery; }
            };
            typedef eosio::multi_index<"experience"_n, experience,
                indexed_by<"second"_n, const_mem_fun<experience, uint64_t, &experience::secondary_key>>
            > experiences;
        // ------------------------------------

        // TABLE aura_spec: lista de todas las auras definidas por una app sobre las props de una mastery
            // scope: contract
            TABLE aura_spec {
                uint64_t          id;
                name            nick;
                uint64_t         app; // table vapaeeaouthor::authors.id
                uint64_t     mastery; // tabla mastery_spec.
                uint64_t  acumulable; // indica la cantidad de veces que se puede acumular
                // TODO: implementar los points: lista de tuplas (prop-level-points-value)
                uint64_t primary_key() const { return id; }
                uint128_t secondary_key() const { return vapaee::combine(app, nick); }
                uint64_t app_key() const { return app; }
            };
            typedef eosio::multi_index<"aura"_n, aura_spec,
                indexed_by<"second"_n, const_mem_fun<aura_spec, uint128_t, &aura_spec::secondary_key>>,
                indexed_by<"app"_n, const_mem_fun<aura_spec, uint64_t, &aura_spec::app_key>>
            > aura_specs;
        // ------------------------------------

        // TABLE aura_fx: las auras que tiene prendidas el usuario
            // scope: user
            // row: significa que el usuario tiene activa esta aura quantity veces
            TABLE aura_fx {
                uint64_t         id;
                uint64_t    mastery;   // mastery_spec
                uint64_t  container;   // container_instance
                // TODO: ver la posibilidad de implementar un cache aca
                uint64_t primary_key() const { return id;  }
            };
            typedef eosio::multi_index<"aurafx"_n, aura_fx> aurafxs;
        // ------------------------------------

    public:
        using contract::contract;

        // 
        ACTION newmastery(
                name author_owner,
                uint64_t author_app,
                name nick,
                name title,
                name desc,
                name table, // only if it is a item mastery "item_spec" or "item_asset". Else leave empty "".
                uint64_t row, // if of item_spec or item_asset
                const std::vector<mastery_property>& mastery_properties) {
            // inline::action(newctner, author_owner, author_app, mastery_name, ?)
            // crear una entrada en mastery_spec con mastery_name
            // iterar sobre los properties y crear una entrada por cada uno en el scope del mastery_spec.id

            // verify if table and row were specified
            name item_spec_table = name{"item_spec"};
            name item_asset_table = name{"item_asset"};
            name no_table = name{""};
            if (table != no_table) {
                eosio_assert(table == item_spec_table || table == item_asset_table, "table MUST be '' or 'item_spec' or 'item_asset'");

                // assert row exist
                if (table == item_spec_table) {
                    item_specs items(get_self(), get_self().value);
                    auto row_spec = items.find(row);
                    eosio_assert(row_spec != items.end(), "Item spec not found");
                }

                if (table == item_asset_table) {
                    stats assets(get_self(), get_self().value);
                    auto row_asset = assets.find(row);
                    eosio_assert(row_asset != assets.end(), "Item asset not found");
                }
            }

            // verify app exists
            slug app_nick = vapaee::get_author_nick(author_app);

            // verify mastery does not exist
            mastery_specs masteries(get_self(), get_self().value);
            auto mastery_index = masteries.template get_index<"second"_n>();
            auto mastery_itr = mastery_index.find( vapaee::combine(author_app, nick) );
            eosio_assert(mastery_itr == mastery_index.end(),
                (string("Mastery ") + nick.to_string() + " already registered for app " + app_nick.to_string()).c_str() );

            // create mastery
            uint64_t id = masteries.available_primary_key();
            masteries.emplace( author_owner, [&]( auto& s ) {
                s.id = id;
                s.nick = nick;
                s.app = author_app;
                s.table = table;
                s.row = row;
            });

            action(
                permission_level{get_self(),"active"_n},
                get_self(),
                "newcontainer"_n,
                std::make_tuple(author_owner, author_app, nick, 0)
            ).send();

            // mastery_properties
            mastery_props properties(get_self(), id);
            for (int i; i<mastery_properties.size(); i++) {
                mastery_property prop = mastery_properties[i];
                properties.emplace( author_owner, [&]( auto& p ) {
                    p.id = properties.available_primary_key();
                    p.title = mastery_properties[i].title;
                    p.desc = mastery_properties[i].desc;
                    p.property = mastery_properties[i].property;
                    p.icon.small = mastery_properties[i].icon.small;
                    p.icon.big = mastery_properties[i].icon.big;
                    p.levels = mastery_properties[i].levels;
                });                
            }
                        
        };

        // new container_spec
        ACTION newcontainer(name author_owner, uint64_t author_app, name nickname, int space) {
            // se fija que no exista en container_spec un nickname para author_app 
            container_specs container_table(get_self(), get_self().value);
            auto index = container_table.template get_index<"second"_n>();
            auto itr = index.find(vapaee::combine(author_owner, nickname));
            eosio_assert(itr == index.end(), (nickname.to_string()  + " already registered as container").c_str());
            // crea en container_spec un row con nickname para author_app
            container_table.emplace( author_owner, [&]( auto& s ) {
                s.id = container_table.available_primary_key();
                s.nick = nickname;
                s.app = author_app;
                s.space = space;
            });
        };

        // --------------------- ITEM ------------------
        ACTION newitem(name author_owner, uint64_t author_app, name nickname, int maxgroup) {
            // verifica que no exista un nickname para el author_app
            item_specs item_table(get_self(), get_self().value);
            auto index = item_table.template get_index<"second"_n>();
            auto itr = index.find(vapaee::combine(author_owner, nickname));
            eosio_assert(itr == index.end(), (nickname.to_string()  + " already registered as item").c_str());
            // crea un nuevo item_spec 
            item_table.emplace( author_owner, [&]( auto& s ) {
                s.id = item_table.available_primary_key();
                s.nick = nickname;
                s.app = author_app;
                s.maxgroup = maxgroup;
            });
        };

        // -- INCOMPLETO --
        ACTION newasset(name author_owner, uint64_t author_issuer, slug_asset maximum_supply, uint64_t spec) {
            // una entrada en item_asset con supply 0
            // verifica que no exista un nickname para el author_app
            stats assets_table(get_self(), get_self().value);
            auto index = assets_table.template get_index<"second"_n>();
            auto itr = index.find(maximum_supply.symbol.code().raw().to128bits());
            eosio_assert(itr == index.end(), (maximum_supply.to_string()  + " already registered as asset").c_str());

            // TODO: tengo que exigir que el maximum_supply.symbol.code().raw() tenga com prefijo al nick del author

            // crea un nuevo item_spec 
            assets_table.emplace( author_owner, [&]( auto& s ) {
                s.id = assets_table.available_primary_key();
                s.supply.amount = 0;
                s.max_supply = maximum_supply;
                s.spec = spec; // table item_spec
                s.publisher = author_issuer; // table vapaeeaouthor::authors.id
                s.block = current_time();
            });
        };

        ACTION issueunits(name to, slug_asset quantity, string memo) {
            // tiene que requerir la firma de quien sea que sea el owner actual del publisher de este asset
            stats assets_table(get_self(), get_self().value);
            auto index = assets_table.template get_index<"second"_n>();
            auto itr = index.find(quantity.symbol.code().raw().to128bits());
            eosio_assert(itr != index.end(), (quantity.to_string()  + " is not registered as item_asset").c_str());
            name owner = vapaee::get_owner_for_author(itr->publisher);
            require_auth(owner);

            eosio_assert( quantity.is_valid(), "invalid quantity" );
            eosio_assert( quantity.amount > 0, "must issue positive quantity" );

            // update total supply
            eosio_assert(quantity.amount <= itr->max_supply.amount - itr->supply.amount, "Can't print over maximun");
            assets_table.modify( *itr, same_payer, [&]( auto& s ) {
                s.supply += quantity;
            });

            add_balance( owner, quantity, owner );
        };

        ACTION transfer( name         from,
                         name         to,
                         slug_asset   quantity,
                         string       memo) {
            // toma los primeros quantity items que encuentre en la tabla item_units
            // itera sobre los resultados y arma un vector con los id de los resultados
            // inline::action (transfunits, from , to, items, memo)

            // aux variables
            slug asset_slug = quantity.symbol.code().raw();
            const char * error_str = (asset_slug.to_string() + " was not registered as item asset").c_str();

            // we search for the asset identityed by quantity.symbol (asset_slug)
            stats assets_table(get_self(), get_self().value);
            auto index = assets_table.template get_index<"second"_n>();
            auto itr = index.find( asset_slug.to128bits() );
            eosio_assert(itr != index.end(), error_str);
            // we make sure we have the correct asset (avoiding 128 bit colusion)
            while (itr->supply.symbol != quantity.symbol && itr != index.end()) {
                itr++;
            }
            eosio_assert(itr != index.end(), error_str);
            eosio_assert(itr->supply.symbol != quantity.symbol, error_str);

            // we search over owner's units of this asset. We put the first quantity.amount units id ina list
            uint64_t asset_id = itr->id;
            vector<uint64_t> units;
            item_units units_from(get_self(), from.value);
            auto units_index = units_from.template get_index<"second"_n>();
            auto units_itr = units_index.lower_bound(asset_id);
            for (int i=0; i<quantity.amount && units_itr != units_index.end(); i++, units_itr++) {
                if (units_itr->asset == asset_id) {
                    units.push_back(units_itr->id);
                }
            }
            eosio_assert(units.size() == quantity.amount, (string("Not enough items of asset ") + asset_slug.to_string()).c_str());

            // we call for a unit list trastaction to be perfpormed
            action(
                permission_level{from,"active"_n},
                get_self(),
                "transfunits"_n,
                std::make_tuple(from, to, quantity, units, memo)
            ).send();
        };

        // -- REVISAR COMPLETITUD --
        ACTION transfunits( name         from,
                            name         to,
                            slug_asset   quantity,
                            const std::vector<uint64_t>& items, // id en la tabla item_slot
                            string       memo ) {
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

            // Encuentro un nuevo slot para cada unidad y a continuación hago un swap
            slotinfo target_slot;
            item_spec spec;
            find_item_spec(quantity.symbol, spec);
            for (int i=0; i<items.size(); i++) {
                find_room_for_unit(to, quantity.symbol, spec, target_slot);
                swap(from, items[i], to, target_slot, ram_payer);
            }
        };

        // -- INCOMPLETO --
        void swap(name from, uint64_t unit, name to, slotinfo target_slot, name ram_payer) {
            if (from == to) {
                /// ---------- TENGO QUE RE VER TODO ESTETEMA ------------

                if (/*los containers origen y destino son diferentes*/) {
                    // hay que aumentar el empty del container origen
                } else {

                }

                // no se crean ni se destruyen rows, sólo se modifican
                // hay que ver si existe algo en el target_slot
                if (true/*existe*/) {
                    // hay que averiguar si ambos unidades pertenecen al mismo item_asset
                    // si es así, hay que preguntar si el item_spec especifica un máximo para el agrupamiento (dif de 1)
                    // si el maxgroup == 0 -> son acumulables
                    // si el maxgroup > 1 -> hay que preguntar si (target_slot.quantity <= maxgroup - origin_slot.quantity)
                    if (/*acumulables*/) {
                        // el dato empty del container debe aumentar (ya que agrupamos dos o más unidaes en un mismo slot)
                        // debe eliminarse la entrada del origin_slot
                    } else {
                        // el dato empty del container permanece intacto
                        // deben modificarse ambas entradas, intercambiando la infoslot en cada una
                    }                    
                } else {
                    // solamente hay que modificar la entrada unit y poner la nueva target_slot ahi
                }
            } else {
                // es una transferencia de un usuario a otro
                // - exigir que el target_slot esté vacío
                // - crear un nuevo row en el item_unit del usuario "to" y ponerle la unidad ahi
                // - eliminar el row del item_unit para el usuario "from"
                // - exigir container.empty > 0
                // - from_container.empty += 1;
                // - target_container.empty -= 1;
            }
        }

        void find_item_spec(const slug_symbol& symbol, item_spec &spec) {

            // search asset identified by symbol
            stats asset_table(get_self(), get_self().value);
            auto index = asset_table.template get_index<"second"_n>();
            auto itr = index.find( symbol.code().raw().to128bits() );

            while (itr != index.end() && itr->supply.symbol.raw() != symbol.code().raw()) {
                itr++;
            }

            eosio_assert(itr != index.end(), (string("no asset was found with the name: ") + symbol.code().raw().to_string()).c_str() );

            // extract spec with the spec_id
            uint64_t spec_id = itr->spec;
            item_specs spec_table(get_self(), get_self().value);
            auto spec_itr = spec_table.get(spec_id, (string("no spec was found for the the name: ") + symbol.code().raw().to_string()).c_str() );

            // poblate answer 
            spec.id = spec_itr.id;
            spec.nick = spec_itr.nick;
            spec.app = spec_itr.app;
            spec.maxgroup = spec_itr.maxgroup;
        }

        void find_app_inventory(name user, uint64_t app, container_instance &container) {
            // find container_spec for app|inventory
            container_specs cont_spec_table(get_self(), get_self().value);
            auto index = cont_spec_table.template get_index<"second"_n>();
            auto itr = index.find( vapaee::combine(app, "inventory"_n.value) );
            eosio_assert(itr != index.end(), "no inventory registered for app");
            
            // find container_instance of the user for that spec
            containers cont_table(get_self(), user.value);
            auto index_inv = cont_table.template get_index<"spec"_n>();
            auto itr_inv = index_inv.find( vapaee::combine(app, "inventory"_n.value) );
            eosio_assert(itr_inv != index_inv.end(), "no inventory registered for app");

            // poblate answer
            container.id = itr_inv->id;
            container.spec = itr_inv->spec;
            container.empty = itr_inv->empty;
            container.space = itr_inv->space;
        }

        // -- INCOMPLETO --
        void find_room_for_unit(name owner, const slug_symbol& symbol, item_spec &spec, slotinfo &slot) {
            // TODO: implementar
            // 
            // tengo que saber dar con la instancia de container del usuario para este tipo de item
            container_instance container;
            find_app_inventory(owner, spec.app, container);

            eosio_assert(container.empty > 0, "User does not have any space left in the app inventory");



            // el container.empty > 0
            // tengo que recorrer los item_units ordenados por posición y filtrado por container
            // buscar el menor posicion que esté disponible
            // poblar el slotinfo con container/newposition
            
            /*


            TABLE container_instance {
                uint64_t      id;
                uint64_t    spec; // table container_specs.id
                int        empty;
                int        space;
                uint64_t primary_key() const { return id;  }
                uint64_t spec_key() const { return spec; }
            };
            typedef eosio::multi_index<"containers"_n, container_instance,
                indexed_by<"spec"_n, const_mem_fun<container_instance, uint64_t, &container_instance::spec_key>>
            > containers;


            TABLE item_unit {
                uint64_t           id;
                uint64_t        asset;  // item_asset
                slotinfo         slot; 
                int          quantity;  // quantity <= item_asset.spec.maxgroup
                uint64_t primary_key() const { return id;  }
                uint64_t asset_key() const { return asset;  }
                uint64_t container_key() const { return slot.container; }
                uint128_t slot_key() const { return slot.to128bits(); }
            };
            typedef eosio::multi_index<"items"_n, item_unit,
                indexed_by<"second"_n, const_mem_fun<item_unit, uint64_t, &item_unit::asset_key>>,
                indexed_by<"container"_n, const_mem_fun<item_unit, uint64_t, &item_unit::container_key>>,
                indexed_by<"slot"_n, const_mem_fun<item_unit, uint128_t, &item_unit::slot_key>>
                
            > item_units;


            */            
        }

        // -- INCOMPLETO --
        ACTION open( name owner, const slug_symbol& symbol, name ram_payer ) {
            // TODO: implementar
        };

        ACTION close( name owner, const slug_symbol& symbol ) {
            require_auth( owner );

            accounts account_table( get_self(), owner.value );
            auto index = account_table.template get_index<"second"_n>();
            auto itr = index.find( symbol.code().raw().to128bits());
            
            // we make sure we have the correct asset (avoiding 128 bit colusion)
            while ( itr != index.end() && itr->balance.symbol != symbol ) {
                itr++;
            }
            
            eosio_assert( itr != index.end(), "Balance row already deleted or never existed. Action won't have any effect." );
            eosio_assert( itr->balance.amount == 0, "Cannot close because the balance is not zero." );

            account_table.erase( *itr );
        };

        // -- INCOMPLETO --
        ACTION burn( name owner, const slug_asset& quantity ) {
            // cualquie rusuario puede quemar una cantidad de sus promias unidades
            // TODO: implementar
        };

        // -------------------- debugging porpuses ---------------------
        // -- INCOMPLETO --
        void dropuser(name owner) {
            // para poder desarrollar más fácil
            users user_table(get_self(), get_self().value);
            auto itr = user_table.find(owner.value);
            eosio_assert(itr != user_table.end(), (string("user not found: ") + owner.to_string()).c_str());
            user_table.erase(itr);

            // TODO: falta borrar todas sus cosas: items, containers, experiences, etc...
        };

        ACTION droptables() {
            name user;
            uint64_t author;
            users user_table(get_self(), get_self().value);
            for (auto itr = user_table.begin(); itr != user_table.end(); itr = user_table.begin()) {
                user = itr->user;
                dropuser(user);

                vector<uint64_t> list;
                list.clear(); // no se, por las dudas
                vapaee::get_authors_for_owner(user, list);
                for (int i=0; i<list.size(); i++) {
                    author = list[i];

                    // borro todos los containers definidos por esta app
                    container_specs cont_table(get_self(), get_self().value);
                    auto index_cont = cont_table.template get_index<"app"_n>();
                    for (auto itr = index_cont.lower_bound(author); itr != index_cont.end() && itr->app == author;) {
                        itr = index_cont.erase(itr);
                    }

                    // borro todos los items_spec definidos por esta app
                    item_specs spec_table(get_self(), get_self().value);
                    auto index_spec = spec_table.template get_index<"app"_n>();
                    for (auto itr = index_spec.lower_bound(author); itr != index_spec.end() && itr->app == author;) {
                        itr = index_spec.erase(itr);
                    }

                    // borro todos los items_asset definidos por este publisher
                    stats cont_spec_table(get_self(), get_self().value);
                    auto index_asset = cont_spec_table.template get_index<"publisher"_n>();
                    for (auto itr = index_asset.lower_bound(author); itr != index_asset.end() && itr->publisher == author;) {
                        itr = index_asset.erase(itr);
                    }

                    // borro todos los mastery_spec definidos por esta app
                    mastery_specs mastery_table(get_self(), get_self().value);
                    auto index_mty = mastery_table.template get_index<"app"_n>();
                    for (auto itr = index_mty.lower_bound(author); itr != index_mty.end() && itr->app == author;) {
                        // borro todos los mastery_prop definidos por este mastery_spec
                        mastery_props props_table(get_self(), itr->id);
                        for (auto prop_itr = props_table.begin(); prop_itr != props_table.end();) {
                            prop_itr = props_table.erase(prop_itr);
                        }

                        itr = index_mty.erase(itr);
                    }
                }

            }
        };

        void add_balance( name owner, slug_asset value, name ram_payer ) {
            accounts account_table( get_self(), owner.value );
            auto index = account_table.template get_index<"second"_n>();
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
            auto index = account_table.template get_index<"second"_n>();
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

        static slug_asset get_supply( name token_contract_account, slug_symbol_code sym_code ) {
            stats asset_table(token_contract_account, token_contract_account.value);
            auto index = asset_table.template get_index<"second"_n>();
            auto itr = index.find( sym_code.raw().to128bits() ); // sym_code.raw().to64bits()

            while (itr != index.end() && itr->supply.symbol.raw() != sym_code.raw()) {
                itr++;
            }

            eosio_assert(itr != index.end(), (string("no asset was found with the name: ") + sym_code.to_string()).c_str() );

            return itr->supply;
        }

        static slug_asset get_balance( name token_contract_account, name owner, slug_symbol_code sym_code ) {

            accounts accounttable( token_contract_account, owner.value );
            auto index = accounttable.template get_index<"second"_n>();
            auto itr = index.find( sym_code.raw().to128bits() );

            while (itr != index.end() && itr->balance.symbol.raw() != sym_code.raw()) {
                itr++;
            }
            
            return itr->balance;
        }

        


/*
-- ACTIONS for developers --
- registerPublisher(owner, publisher, prefix, isapp): registra un publisher co un pefix (que deberá usar en el slug de las cartas y albums que publique)
- addInventory(owner, publisher, settings...) // solo si publisher isapp
- registerItemSpec(publisher, associated_app, settings...)
- registerContainerSpec(publisher, settings...)

-- ACTIONS for clients --
- registerItemSpec(publisher, "cardsntokens", settings...) // se repite porque en C&T sería el cliente que quiere hacer una carta nueva
- issueItems(publisher, quantity);
- transferItems(from, to, units); // el destinatario debe tener espacio en su inventario de la aplicación asociada al item
- moveItem() // swap del contenido de dos slots en mysql


*/


/*

-- Items & Containers --

-- Market --
- subasta, ordenes de compra o venta

-- Masteries --
-- Masteries: Auras --
-- Masteries: Tokens --

-- Gaming --
- ranking

-- Tournament --

-- Dice -- servicio de randomization con parametros para otros smart contracts.


*/
};

}; // namespace