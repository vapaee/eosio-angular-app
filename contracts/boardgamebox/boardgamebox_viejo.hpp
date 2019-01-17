#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <vapaee/slug_asset.hpp>
#include <vapaee/datatypes.hpp>
#include <vapaee/vapaee_utils.hpp>

#define inventary_default_space 8
#define NULL_MASTERY 0xFFFFFFFFFFFFFFFF


using namespace eosio;
namespace vapaee {

CONTRACT boardgamebox : public eosio::contract {

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
            // scope: app (unit.asset.spec.app)
            // row: representa un slot conteniendo quantity unidades del item definido por asset en la posición position
            TABLE item_unit {
                uint64_t           id;
                name            owner;
                uint64_t        asset;  // item_asset
                slotinfo         slot; 
                int          quantity;  // quantity <= item_asset.spec.maxgroup
                uint64_t primary_key() const { return id;  }
                uint64_t asset_key() const { return asset;  }
                uint64_t container_key() const { return slot.container; }
                uint128_t slot_key() const { return slot.to128bits(); }
                uint64_t owner_key() const { return slot.container; }
            };
            typedef eosio::multi_index<"items"_n, item_unit,
                indexed_by<"second"_n, const_mem_fun<item_unit, uint64_t, &item_unit::asset_key>>,
                indexed_by<"container"_n, const_mem_fun<item_unit, uint64_t, &item_unit::container_key>>,
                indexed_by<"slot"_n, const_mem_fun<item_unit, uint128_t, &item_unit::slot_key>>,
                indexed_by<"owner"_n, const_mem_fun<item_unit, uint64_t, &item_unit::owner_key>>
                
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
                vector<uint64_t> slots;
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
                uint64_t     mastery; // table vapaeeaouthor::authors.id
                int            space; // espacio base que tendrá el container cuando se lo instancie para un usuario. 0 para infinito.
                uint64_t primary_key() const { return id;  }
                uint128_t secondary_key() const { return vapaee::combine(app, nick); }
                uint64_t app_key() const { return app; }
                uint64_t mastery_key() const { return mastery; }
            };
            typedef eosio::multi_index<"contspec"_n, container_spec,
                indexed_by<"second"_n, const_mem_fun<container_spec, uint128_t, &container_spec::secondary_key>>,
                indexed_by<"app"_n, const_mem_fun<container_spec, uint64_t, &container_spec::app_key>>,
                indexed_by<"mastery"_n, const_mem_fun<container_spec, uint64_t, &container_spec::mastery_key>>
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
                uint64_t   container; // container_spec
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
                name          table; // table affected: "", "itemunit", "itemasset"
                uint64_t        row; // row.id
                    // "itemunit": esta unidad está tuneada
                    // "itemasset": esta maestría es para tunear un objeto identificado con un slug item_asset[row].supply.symbol.row()
                // TODO: ver la posibilidad de implementar un cache aca
                uint64_t primary_key() const { return id; }
                uint64_t mastery_key() const { return mastery; }
            };
            typedef eosio::multi_index<"experience"_n, experience,
                indexed_by<"mastery"_n, const_mem_fun<experience, uint64_t, &experience::mastery_key>>
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

        // -- INCOMPLETO --
        ACTION newapp(name owner, uint64_t app, int invespace) {
            action(
                permission_level{owner,"active"_n},
                get_self(),
                "newcontainer"_n,
                std::make_tuple(owner, app, NULL_MASTERY, "inventory", invespace)
            ).send();

            action(
                permission_level{owner,"active"_n},
                get_self(),
                "newcontainer"_n,
                std::make_tuple(owner, app, NULL_MASTERY, "deposit", 0)
            ).send();
        }

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

            
            container_specs container_table(get_self(), get_self().value);
            uint64_t mastery_id = masteries.available_primary_key();
            uint64_t container_id = container_table.available_primary_key();

            // create mastery
            masteries.emplace( author_owner, [&]( auto& s ) {
                s.id = mastery_id;
                s.nick = nick;
                s.container = container_id;
                s.app = author_app;
                s.table = table;
                s.row = row;
            });

            action(
                permission_level{get_self(),"active"_n},
                get_self(),
                "newcontainer"_n,
                std::make_tuple(author_owner, author_app, mastery_id, nick, 0)
            ).send();

            // mastery_properties
            mastery_props properties(get_self(), mastery_id);
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
        ACTION newcontainer(name author_owner, uint64_t author_app, uint64_t mastery, name nickname, int space) {
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
                s.mastery = mastery;
            });
        };

        // --------------------- ITEM ------------------
        ACTION newitem(name author_owner, uint64_t author_app, name nickname, int maxgroup) {
            // exigir al firma de author_owner y que sea el owner del author
            name owner = vapaee::get_owner_for_author(asset.publisher);
            require_auth(owner);
            eosio_assert(owner == author_owner, "given author_owner is not the current owner of author");
            
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

            slug       asset_slug = quantity.symbol.code().raw();
            item_asset asset;
            item_spec  spec;
            item_unit units;
            boardgamebox::get_item_spec_for_slag(asset_slug, asset, spec);

            eosio_assert( quantity.is_valid(), "invalid quantity" );
            eosio_assert( quantity.amount > 0, "must issue positive quantity" );            

            // tiene que requerir la firma de quien sea que sea el owner actual del publisher de este asset
            name owner = vapaee::get_owner_for_author(asset.publisher);
            require_auth(owner);

            // update total supply
            stats assets_table(get_self(), get_self().value);
            eosio_assert(quantity.amount <= asset.max_supply.amount - asset.supply.amount, "Can't print over maximun");
            assets_table.modify( asset, same_payer, [&]( auto& s ) {
                s.supply.amount += quantity.amount;
            });

            auto ram_payer = has_auth( to ) ? to : owner;
            add_balance(to, quantity, ram_payer);

            // cargo un item_unit con los datos del issue
            units.owner = owner;
            units.asset = asset.id;
            units.quantity = quantity.amount;
            // hago instanciar en memoria un item_unit con estos datos en un slot del container "deposit" de este usuario
            put_slot_in_deposit(to, quantity.symbol, spec, units);
        };

        static void get_mastery(uint64_t mastery_id, mastery_spec &mastery) {
            // find container_spec for app|inventory
            name _self = "boardgamebox"_n;
            mastery_specs cont_spec_table(_self, _self.value);
            auto itr = cont_spec_table.find( mastery_id );
            eosio_assert(itr != cont_spec_table.end(), "no mastery registered for id");
            
            // poblate answer
            mastery.id = itr->id;
            mastery.nick = itr->nick;
            mastery.app = itr->app;
            mastery.table = itr->table;
            mastery.row = itr->row;
        }

        static void find_app_inventory(name user, uint64_t app, container_spec &spec, container_instance &container) {
            // find container_spec for app|inventory
            boardgamebox::find_app_container(user, app, "inventory"_n, spec ,container);
        }

        static void find_app_container(name user, uint64_t app, name nick, container_spec &spec, container_instance &container) {
            // find container_spec for app|container
            name _self = "boardgamebox"_n;
            container_specs cont_spec_table(_self, _self.value);
            auto index = cont_spec_table.template get_index<"second"_n>();
            auto itr = index.find( vapaee::combine(app, nick) );
            eosio_assert(itr != index.end(), "no container registered for app");
            
            // find container_instance of the user for that spec
            containers cont_table(_self, user.value);
            auto index_inv = cont_table.template get_index<"spec"_n>();
            auto itr_inv = index_inv.find( vapaee::combine(app, nick) );
            eosio_assert(itr_inv != index_inv.end(), "no container registered for app");

            // poblate answer
            spec.id = itr->id;
            spec.nick = itr->nick;
            spec.app = itr->app;
            spec.mastery = itr->mastery;
            spec.space = itr->space;

            container.id = itr_inv->id;
            container.spec = itr_inv->spec;
            container.empty = itr_inv->empty;
            container.space = itr_inv->space;
            container.slots = itr_inv->slots;
        }

        static void get_item_asset_for_slag(const slug &asset_slug, item_asset &asset) {
            // aux variables
            const char * error_str = (asset_slug.to_string() + " was not registered as item asset").c_str();
            // we search for the asset identityed by quantity.symbol (asset_slug)
            name _self = "boardgamebox"_n;
            stats assets_table(_self, _self.value);
            auto index = assets_table.template get_index<"second"_n>();
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

        static void get_item_spec_for_slag(const slug &asset_slug, item_asset &asset, item_spec &spec) {
            name _self = "boardgamebox"_n;
            item_specs specs_table(_self, _self.value);
            boardgamebox::get_item_asset_for_slag(asset_slug, asset);
            auto itr = specs_table.find(asset.spec);
            eosio_assert(itr != specs_table.end(), "asset point to a null spec");

            spec.id = itr->id;
            spec.nick = itr->nick;
            spec.app = itr->app;
            spec.maxgroup = itr->maxgroup;
        }
        
        static void collect_units_for_asset(
            name user,
            const slug_asset &quantity,
            vector<uint64_t> units,
            vector<int> quantities) {
            // ------------------------------

            // aux variables
            slug asset_slug = quantity.symbol.code().raw();
            item_asset asset;
            item_spec spec;
            boardgamebox::get_item_spec_for_slag(asset_slug, asset, spec);

            // we search over owner's units of this asset. We put the first quantity.amount units id ina list
            uint64_t asset_id = asset.id;
            name _self = "boardgamebox"_n;
            item_units units_table(_self, spec.app);
            int plus_quantity, listed;
            auto units_index = units_table.template get_index<"second"_n>();
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

        ACTION transfer( name         from,
                         name         to,
                         slug_asset   quantity,
                         string       memo) {
            vector<uint64_t> units;
            vector<int> quantities;
            boardgamebox::collect_units_for_asset(from, quantity, units, quantities);

            // we call for a unit list trastaction to be perfpormed
            action(
                permission_level{from,"active"_n},
                get_self(),
                "transfunits"_n,
                std::make_tuple(from, to, quantity, units, quantities, memo)
            ).send();
        };

        // -- REVISAR COMPLETITUD --
        ACTION transfunits( name         from,
                            name         to,
                            slug_asset   quantity,
                            const std::vector<uint64_t>& items, // id en la tabla item_slot
                            vector<int> quantities, // 
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
            boardgamebox::get_item_spec_for_slag(quantity.symbol.code().raw(), asset, spec);
            for (int i=0; i<items.size(); i++) {
                find_inventory_slot(to, quantity.symbol, spec, unit);
                swap(from, items[i], spec.app, quantities[i], to, unit.slot, ram_payer);
            }
        };

        // -- INCOMPLETO (FASE 2) --
        ACTION newuserexp (name user, uint64_t mastery, name table, uint64_t row, name ram_payer) {
            require_auth(user);

            // get next container id
            containers user_containers(get_self(), user.value);
            uint64_t container_id = user_containers.available_primary_key();

            mastery_specs registered_mastery(get_self(), get_self().value);
            auto reg_mastery_itr = registered_mastery.find(mastery);
            uint64_t container_spec_id = reg_mastery_itr->container;

            container_specs registered_cont(get_self(), get_self().value);
            auto reg_cont_index = registered_cont.template get_index<"app"_n>();
            auto reg_cont_itr = reg_cont_index.lower_bound(reg_mastery_itr->app);

            // call for container creation
            action(
                permission_level{user,"active"_n},
                get_self(),
                "newusercont"_n,
                std::make_tuple(user, container_spec_id, ram_payer)
            ).send();

            // verify if table and row were specified
            name item_unit_table = name{"itemunit"};
            name item_asset_table = name{"itemasset"};
            name no_table = name{""};
            if (table != no_table) {
                eosio_assert(table == item_unit_table || table == item_asset_table, "table MUST be '' or 'itemunit' or 'itemasset'");

                // assert row exist
                if (table == item_unit_table) {
                    item_units items(get_self(), reg_mastery_itr->app);
                    auto row_unit = items.find(row);
                    eosio_assert(row_unit != items.end(), "Item unit not found");
                    eosio_assert(row_unit->quantity == 1, "Item Mastery can only affect units with quantityt exacly 1");
                }

                if (table == item_asset_table) {
                    stats assets(get_self(), get_self().value);
                    auto row_asset = assets.find(row);
                    eosio_assert(row_asset != assets.end(), "Item asset not found");
                }

                // TODO: hay que checkear que el mastery_spec tiene en sus table y row el corresponeidnte
            }

            // crear una experience // reg_mastery_itr
            experiences user_experiences(get_self(), user.value);
            auto reg_exp_index = user_experiences.template get_index<"mastery"_n>();
            auto user_exp_itr = reg_exp_index.find(mastery);
            eosio_assert(user_exp_itr == reg_exp_index.end(), "experience already exist for this user");

            user_experiences.emplace(ram_payer, [&](auto&s){
                s.id = user_experiences.available_primary_key();
                s.mastery = mastery;
                s.container = container_id;
                s.table = table;
                s.row = row;
            });

        }
        
        ACTION newusercont (name user, uint64_t spec, name ram_payer) {
            require_auth(user);

            container_specs specs_table(get_self(), get_self().value);
            auto spec_itr = specs_table.find(spec);
            eosio_assert(spec_itr != specs_table.end(), "container_spec not found");

            containers user_containers(get_self(), user.value);
            auto reg_cont_index = user_containers.template get_index<"spec"_n>();
            auto user_cont_itr = reg_cont_index.find(spec);
            eosio_assert(user_cont_itr == reg_cont_index.end(), "container already exist for this user");

            user_containers.emplace(ram_payer, [&](auto&s){
                s.id = user_containers.available_primary_key();
                s.spec = spec_itr->id;
                s.space = spec_itr->space;
                s.empty = spec_itr->space;
            });
        }

        ACTION newuser4app (name user, uint64_t app, name ram_payer) {
            // El objetivo es crearle al usuario todas las instancias de containers y masteries registrados por esa app
            require_auth(user);

            mastery_specs registered_mastery(get_self(), get_self().value);
            auto reg_mastery_index = registered_mastery.template get_index<"app"_n>();
            
            container_specs registered_cont(get_self(), get_self().value);
            auto reg_cont_index = registered_cont.template get_index<"app"_n>();


            experiences user_experiences(get_self(), user.value);
            auto reg_exp_index = user_experiences.template get_index<"mastery"_n>();
            // auto user_exp_itr = reg_exp_index.begin();        
            // iterar sobre todos los mastery_spec registrados por esta app
            for (
                auto reg_mastery_itr = reg_mastery_index.lower_bound(app);
                reg_mastery_itr != reg_mastery_index.end() && reg_mastery_itr->app == app;
                reg_mastery_itr++) {

                // verificar si el usuario ya tine una experiencia para esta mastery
                if (reg_mastery_itr->table.value != (uint64_t)0) {
                    // skip this one because is an item mastery (not an app mastery)
                    continue;
                }

                auto user_exp_itr = reg_exp_index.find(reg_mastery_itr->id);
                if (user_exp_itr == reg_exp_index.end()) {
                    // we call for a unit list trastaction to be perfpormed
                    action(
                        permission_level{user,"active"_n},
                        get_self(),
                        "newuserexp"_n,
                        std::make_tuple(user, reg_mastery_itr->id, ram_payer)
                    ).send();
                }
            }

            containers user_containers(get_self(), user.value);
            auto cont_index = user_containers.template get_index<"spec"_n>();
            // auto user_cont_itr = cont_index.begin();
            // iterar sobre todos los container_spec registrados por esta app
            for (
                auto reg_cont_itr = reg_cont_index.lower_bound(app);
                reg_cont_itr != reg_cont_index.end() && reg_cont_itr->app == app;
                reg_cont_itr++ ) {

                // verificar que no pertenezca a una mastery
                if (reg_cont_itr->mastery != NULL_MASTERY) {
                    // skip this one because is a mastery container (not an app container)
                    continue;
                }

                auto user_cont_itr = cont_index.find(reg_cont_itr->id);
                if (user_cont_itr == cont_index.end()) {
                    // we call for a unit list trastaction to be perfpormed
                    action(
                        permission_level{user,"active"_n},
                        get_self(),
                        "newusercont"_n,
                        std::make_tuple(user, reg_cont_itr->id, ram_payer)
                    ).send();
                }
            }
        };
        
        void swap(
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
            stats assets_table(get_self(), get_self().value);
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
        }

        void put_slot_in_deposit(name owner, const slug_symbol& symbol, const item_spec &spec, item_unit &units) {
            // primero obtengo la instancia de container para el usuario
            container_specs contspec_table(get_self(), get_self().value);
            auto index_deposit = contspec_table.template get_index<"second"_n>();
            auto deposit_spec_itr = index_deposit.find(vapaee::combine(spec.app, "deposit"_n));
            eosio_assert(deposit_spec_itr != index_deposit.end(), "app didn't register deposit inventory");

            containers cont_table(get_self(), owner.value);
            auto index_cont = cont_table.template get_index<"spec"_n>();
            auto deposit_itr = index_cont.find(deposit_spec_itr->id);
            eosio_assert(deposit_itr != index_cont.end(), "user does not have deposit container");

            item_units units_table(get_self(), spec.app);
            auto unit_itr = units_table.begin();

            uint64_t slot_id;
            // si tiene slots vacios en lista
            if (deposit_itr->slots.size() > 0) {
                // saco el primero (pop)
                slot_id = deposit_itr->slots[deposit_itr->slots.size()-1];
                cont_table.modify( *deposit_itr, same_payer, [&](auto &a) {
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
                    a.slot.container = units.slot.container;
                    a.slot.position = units.slot.position;
                });                
            }
        }

        void find_inventory_slot(name owner, const slug_symbol& symbol, const item_spec &spec, item_unit & unit) {
            // tengo que saber dar con la instancia de container del usuario para este tipo de item
            container_instance inventory;
            container_spec inv_spec;
            boardgamebox::find_app_inventory(owner, spec.app, inv_spec, inventory);
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
                auto unit_index = units_table.template get_index<"slot"_n>();
                auto unit_itr = unit_index.upper_bound(inventory.id);
                uint64_t next_position = 0;
                for ( ; unit_itr != unit_index.end(); unit_itr++) {
                    // para cada item_unit del inventario pedirle su posición y quedarme con la pos más baja+1
                    if (unit_itr->slot.container == inventory.id) {
                        next_position = unit_itr->slot.position + 1;
                        break;
                    }
                }

                // instancio un item_unit con el owner, id, slot.position encontrada, quantity = 0 
                unit_id = units_table.available_primary_key();
                units_table.emplace(owner, [&](auto &a){
                    a.id = unit_id;
                    a.owner = owner;
                    a.slot.container = inventory.id;
                    a.slot.position = next_position;
                    a.quantity = 0;
                });

                // tengo que agregar el item_unit.id a la lista de cointainer.slots porque sigue vacío
                containers cont_table(get_self(), owner.value);
                cont_table.modify(inventory, owner, [&](auto &a){
                    a.slots.push_back(unit_id);
                });
            }                        
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

        ACTION burn( name owner, const slug_asset& quantity, slotinfo & slot) {
            require_auth(owner);

            slug asset_slug = quantity.symbol.code().raw();
            item_asset asset;
            item_spec spec;
            boardgamebox::get_item_spec_for_slag(asset_slug, asset, spec);

            item_units units_table(get_self(), spec.app);
            auto slot_index = units_table.template get_index<"slot"_n>();
            auto unit = slot_index.find(slot.to128bits());
            eosio_assert(unit != slot_index.end(), "slot does not point to a unit for this slug_asset");

            units_table.modify(*unit, owner, [&](auto &a){
                a.quantity = 0;
            });

            containers cont_table(get_self(), owner.value);
            auto cont = cont_table.find(unit->slot.container);
            eosio_assert(cont != cont_table.end(), "slot does not point to a container for this user");
            cont_table.modify(*cont, owner, [&](auto&a){
                a.slots.push_back(unit->id);
            });

            sub_balance(owner, quantity);
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