#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <vapaee/slug_asset.hpp>

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
            uint128_t secondary_key() const { return boardbagebox::combine(nick, app); }
        };
        typedef eosio::multi_index<"containerspec"_n, container_spec,
            indexed_by<"second"_n, const_mem_fun<container_spec, uint128_t, &container_spec::secondary_key>>
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
            uint128_t secondary_key() const { return boardbagebox::combine(nick, app); }
        };
        typedef eosio::multi_index<"itemspec"_n, item_spec,
            indexed_by<"second"_n, const_mem_fun<item_spec, uint128_t, &item_spec::secondary_key>>
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
        };
        typedef eosio::multi_index<"stat"_n, item_asset,
            indexed_by<"second"_n, const_mem_fun<item_asset, uint128_t, &item_asset::secondary_key>>
        > stats;


        // Mastery ------------------------------------------------
        // -- lista de todas las maestrías definidas por una app --
        // scope: contract
        TABLE mastery_spec {
            uint64_t          id;
            name            nick; 
            uint64_t         app; // table vapaeeaouthor::authors.id
            name           table; // table affected: "", "item_spec", "item_asset"
            uint64_t         row; // row.id 
                // "item_spec": esta maestría es para tunear un objeto de tipo descrito en la fila row
                // "item_asset": esta maestría es para tunear un objeto identificado con un slug item_asset[row].supply.symbol.row()
            uint64_t primary_key() const { return id; }
            uint128_t secondary_key() const { return boardbagebox::combine(nick, app); }
        };
        typedef eosio::multi_index<"mastery"_n, mastery_spec,
            indexed_by<"second"_n, const_mem_fun<mastery_spec, uint128_t, &mastery_spec::secondary_key>>
        > mastery_specs;        

        // -- lista de properties que tiene una mastery --
        // scope: mastery
        // row: representa una property de la maestría
        TABLE mastery_prop {
            uint64_t          id;
            slug        property; 
            // TODO: no se como voy a representar la evaluación de cada una de sus leves
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
            uint128_t secondary_key() const { return boardbagebox::combine(nick, app); }
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

        // --------------------- APPS ------------------
        ACTION newapp(name author_owner,
                        uint64_t author_app,
                        int inventory_space,
                        const std::vector<mastery_prop>& mastery_properties
        ) {
            // inline::action(newctner, author_owner, author_app, "inventory", inventory_space)
            // inline::action(newmtery, author_owner, author_app, "mastery", mastery_properties)
        };

        // 
        ACTION newmastery(name author_owner, uint64_t author_app, slug mastery_name, const std::vector<mastery_prop>& mastery_properties) {
            // inline::action(newctner, author_owner, author_app, mastery_name, ?)
            // crear una entrada en mastery_spec con mastery_name
            // iterar sobre los properties y crear una entrada por cada uno en el scope del mastery_spec.id
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

        ACTION issue(name to, slug_asset quantity, string memo){
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

        ACTION droptables(name owner) {
            // para poder desarrollar más fácil
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

        static uint128_t combine( name nick, uint64_t author ) {
            return (uint128_t{nick.value} << 64) | uint128_t{author};
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