#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <vapaee/slug_asset.hpp>
#include <vapaee/mastery_property.hpp>
#include <vapaee/vapaee_aux_functions.hpp>

#define inventary_default_space 8

using namespace eosio;
namespace vapaee {

CONTRACT boardbagebox : public eosio::contract {

    private:
        // -- Balance de unidades --
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

        // -- donde están las unidades de items --
        // scope: user
        // row: representa un slot conteniendo quantity unidades del item definido por asset en la posición position
        TABLE item_unit {
            uint64_t           id;
            uint64_t        asset;  // item_asset
            uint64_t    container;  // container_instance 
            uint64_t     position;
            int          quantity;  // quantity <= item_asset.spec.maxgroup
            uint64_t primary_key() const { return id;  }
        };
        typedef eosio::multi_index<"items"_n, item_unit> item_units;

        // -- Los contenedores que tiene un usuario --
        // scope: user
        // row: representa un container para ese usuario que indica cuantos lugares libres tienen y que espacio total tiene
        TABLE container_instance {
            uint64_t      id;
            uint64_t    spec; // table container_specs.id
            int        empty;
            int        space;
            uint64_t primary_key() const { return id;  }
        };
        typedef eosio::multi_index<"containers"_n, container_instance> containers;

        // lista de todos los usuarios que usaron el sistema. es para poder iterar sobre todos los nombres
        // scope: contract
        // row: representa un usuario
        TABLE user_reg {
            name user;
            uint64_t primary_key() const { return user.value;  }
        };
        typedef eosio::multi_index<"users"_n, user_reg> users;

        // -- Lista de todos los contenedores definidos. Cada app tiene varios contenedores definidos. --
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
        typedef eosio::multi_index<"containerspec"_n, container_spec,
            indexed_by<"second"_n, const_mem_fun<container_spec, uint128_t, &container_spec::secondary_key>>,
            indexed_by<"app"_n, const_mem_fun<container_spec, uint64_t, &container_spec::app_key>>
        > container_specs; 

        // -- Lista de todos los TIPOS de items que existen. Cada App define varios tipos de items --
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

        // -- Lista de todos los ITEMS definidos por alguien (los cuales tienen un TIPO de item) --
        // scope: contract
        // row: representa un item, del cual existe una cantidad supply de unidades que no supera el max_supply
        TABLE item_asset {
            uint64_t              id;
            slug_asset        supply;
            slug_asset    max_supply;
            uint64_t            spec; // table item_spec
            uint64_t       publisher; // table vapaeeaouthor::authors.id
            uint32_t           block; // tapos_block_num() in eosiolib/transaction.hpp https://eosio.stackexchange.com/a/759
            uint64_t primary_key() const { return id;  }
            uint128_t secondary_key() const { return supply.symbol.code().raw().to128bits(); }
            uint64_t publisher_key() const { return publisher; }
        };
        typedef eosio::multi_index<"stat"_n, item_asset,
            indexed_by<"second"_n, const_mem_fun<item_asset, uint128_t, &item_asset::secondary_key>>,
            indexed_by<"publisher"_n, const_mem_fun<item_asset, uint64_t, &item_asset::publisher_key>>
        > stats;


        // Mastery ------------------------------------------------
        // -- lista de todas las maestrías definidas por una app --
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
        };
        typedef eosio::multi_index<"mastery"_n, mastery_spec,
            indexed_by<"second"_n, const_mem_fun<mastery_spec, uint128_t, &mastery_spec::secondary_key>>
        > mastery_specs;        

        // -- lista de properties que tiene una mastery --
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

        // -- Los contenedores que tiene un usuario --
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
        
        // -- lista de todas las auras definidas por una app sobre las props de una mastery --
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
        };
        typedef eosio::multi_index<"aura"_n, aura_spec,
            indexed_by<"second"_n, const_mem_fun<aura_spec, uint128_t, &aura_spec::secondary_key>>
        > aura_specs;
        
        // -- las auras que tiene prendidas el usuario --
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
                    p.property = mastery_properties[i].property;
                    
                    // uint64_t                   id;
                    // name                    title; // localstrings.strings.key
                    // name                     desc; // localstrings.strings.key
                    // slug                 property;
                    // iconinfo                 icon;
                    // std::vector<levelinfo> levels;
                });                
            }
                        
        };

        // new container_spec
        ACTION newcontainer(name author_owner, uint64_t author_app, name nickname, int space) {
            // se fija que no exista en container_spec un nickname para author_app 
            // crea en container_spec un row con nickname para author_app
        };

        // --------------------- ITEM ------------------
        ACTION newitem(name author_owner, uint64_t author_app, name nickname) {
            // verifica que no exista un nickname para el author_app
            // crea un nuevo item_spec 
        };

        ACTION newasset(name author_owner, uint64_t author_issuer, slug_asset maximum_supply, uint64_t spec) {
            // una entrada en item_asset con supply 0
            // una entrada en account con balance 0 y una entrada en authorship con balance 100 
        };

        ACTION issueunits(name to, slug_asset quantity, string memo){
            // crea quantity unidades para to (no necesita tener espacio)
            // tiene que requerir la firma de quien sea que sea el owner actual del publisher de este asset
        };

        ACTION transfer( name         from,
                         name         to,
                         slug_asset   quantity,
                         string       memo
        ){
            // toma los primeros quantity items que encuentre en la tabla item_slot
            // itera sobre los resultados y arma un vector con los id de los resultados
            // inline::action (transfunits, from , to, items, memo)
        };

        ACTION transfunits( name         from,
                            name         to,
                            const std::vector<uint64_t>& items, // id en la tabla item_slot
                            string       memo ){
            // tiene que descontar del balance de from y aumentarlo en to (table account)
            // tiene que buscar en item_slot del from y sacar quantity unidades
            // tiene que buscar en item_slot del to y sacar verificar que tiene empty <= quantity
            // hubicar uno por uno los quantity slots y hacer el cambio de cointainer.id y position
            // actualizar el empty para ambos containers (el del from y el del to)
            // items.size()
            // items[i]
        };        

        ACTION open( name owner, const slug_symbol& symbol, name ram_payer ) {

        };

        ACTION close( name owner, const slug_symbol& symbol ) {

        };

        ACTION burn( name owner, const slug_asset& quantity ) {
            // cualquie rusuario puede quemar una cantidad de sus promias unidades
        };

        // -------------------- debugging porpuses ---------------------
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
                    item_asset asset_table(get_self(), get_self().value);
                    auto index_asset = asset_table.template get_index<"publisher"_n>();
                    for (auto itr = index_asset.lower_bound(author); itr != index_asset.end() && itr->publisher == author;) {
                        itr = index_asset.erase(itr);
                    }

                    // borro todos los mastery_spec definidos por esta app
                    mastery_spec mastery_table(get_self(), get_self().value);
                    auto index_mty = mastery_table.template get_index<"app"_n>();
                    for (auto itr = index_mty.lower_bound(author); itr != index_mty.end() && itr->app == author;) {
                        // borro todos los mastery_prop definidos por este mastery_spec
                        mastery_props props_table(get_self(), itr->id);
                        for (auto prop_itr = props_table.begin(); itr != props_table.end();) {
                            prop_itr = props_table.erase(prop_itr);
                        }

                        itr = index_mty.erase(itr);
                    }


                    /*
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
        };
        typedef eosio::multi_index<"mastery"_n, mastery_spec,
            indexed_by<"second"_n, const_mem_fun<mastery_spec, uint128_t, &mastery_spec::secondary_key>>
        > mastery_specs;        


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

                    */
                    


                }

            }
        };
    

        static slug_asset get_supply( name token_contract_account, slug_symbol_code sym_code )
        {
            stats statstable( token_contract_account, sym_code.raw().to64bits() );
            auto index = statstable.template get_index<"second"_n>();
            auto itr = index.find( sym_code.raw().to128bits() ); // sym_code.raw().to64bits()

            while (itr != index.end() && itr->supply.symbol.raw() != sym_code.raw()) {
                itr++;
            }

            eosio_assert(itr != index.end(), (string("no asset was found named ") + sym_code.to_string()).c_str() );

            return itr->supply;
        }

        static slug_asset get_balance( name token_contract_account, name owner, slug_symbol_code sym_code )
        {

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




*/
};

}; // namespace