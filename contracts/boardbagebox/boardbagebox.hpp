#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;

CONTRACT boardbagebox : public eosio::contract {
    public:
        using contract::contract;

        // --------------------- APPS ------------------
        ACTION createapp(name author_owner,
                        uint64_t author_app,
                        int inventory_space,
                        const std::vector<mastery_prop>& mastery_properties
        ) {
            // inline::action(createctner, author_owner, author_app, "inventory", inventory_space)
            // inline::action(createmtery, author_owner, author_app, "mastery", mastery_properties)
        };

        // 
        ACTION createmtery(name author_owner, uint64_t author_app, slug mastery_name, const std::vector<mastery_prop>& mastery_properties) {
            // inline::action(createctner, author_owner, author_app, mastery_name, ?)
            // crear una entrada en mastery_spec con mastery_name
            // iterar sobre los properties y crear una entrada por cada uno en el scope del mastery_spec.id
        };

        // create container_spec
        ACTION createctner(name author_owner, uint64_t author_app, name nickname, int space) {
            // se fija que no exista en container_spec un nickname para author_app 
            // crea en container_spec un row con nickname para author_app
        };

        // --------------------- ITEM ------------------
        ACTION createitem(name author_owner, uint64_t author_app, name nickname) {
            // verifica que no exista un nickname para el author_app
            // crea un nuevo item_spec 
        };

        ACTION createasset(name author_owner, uint64_t author_issuer, slug_asset maximum_supply, uint64_t spec) {
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

        static int get_authorship( name token_contract_account, name owner, slug_symbol_code sym_code )
        {

            accounts accounttable( token_contract_account, owner.value );
            auto index = accounttable.template get_index<"second"_n>();
            auto itr = index.find( sym_code.raw().to128bits() );

            while (itr != index.end() && itr->balance.symbol.raw() != sym_code.raw()) {
                itr++;
            }
            
            return itr->authorship;
        }        

        static uint128_t combine( name nick, uint64_t author ) {
            return (uint128_t{nick.value} << 64) | uint128_t{author};
        } 
    private:
        // balances -------------
        TABLE account { // scope es el user
            uint64_t           id;
            uint64_t        asset; // item_asset.id
            slug_asset    balance;
            uint64_t primary_key() const { return id;  }
            uint128_t secondary_key() const { return balance.symbol.code().raw().to128bits(); }
        };
        typedef eosio::multi_index<"accounts"_n, account, 
            indexed_by<"second"_n, const_mem_fun<account, uint128_t, &account::secondary_key>>
        > accounts;
        
        TABLE authorship { // scope es el vapaeeaouthor::authors.id
            uint64_t           id;
            uint64_t        asset; // table 
            slug_asset    balance;
            uint64_t primary_key() const { return id;  }
            uint128_t secondary_key() const { return balance.symbol.code().raw().to128bits(); }
        };
        typedef eosio::multi_index<"authorship"_n, authorship,
            indexed_by<"second"_n, const_mem_fun<authorship, uint128_t, &authorship::secondary_key>>
        > authors;

        // ------------------- where are the items -------------------------------
        TABLE item_slot { // scope es el user
            uint64_t           id;
            uint64_t        asset;  // item_asset.id
            uint64_t    container;
            uint64_t     position;
            int          quantity;  // quantity <= item_asset.spec.maxgroup
            uint64_t primary_key() const { return id;  }
        };
        typedef eosio::multi_index<"items"_n, item_slot> item_slots;

        TABLE container { // scope es el user
            uint64_t      id;
            uint64_t    spec; // table container_specs.id
            int        empty;
            int        space;
            uint64_t primary_key() const { return id;  }
        };
        typedef eosio::multi_index<"containers"_n, container,
            indexed_by<"second"_n, const_mem_fun<container, uint128_t, &container::secondary_key>>
        > containers;


        // -------------------- type registration ---------------------------------
        TABLE container_spec { // scope: self
            uint64_t          id;
            string      nickname;
            int            space;
            uint64_t         app; // table vapaeeaouthor::authors.id
            uint64_t primary_key() const { return id;  }
            uint64_t secondary_key() const { return app; }
        };
        typedef eosio::multi_index<"containerspec"_n, container_spec,
            indexed_by<"second"_n, const_mem_fun<container_spec, uint64_t, &container_spec::secondary_key>>
        > container_specs; 

        TABLE item_asset { // scope: self
            uint64_t              id;
            uint64_t            spec; // table item_spec
            slug_asset        supply;
            slug_asset    max_supply;
            uint64_t       publisher; // table vapaeeaouthor::authors.id
            uint128_t          block; // tapos_block_num() in eosiolib/transaction.hpp https://eosio.stackexchange.com/a/759
            uint64_t primary_key() const { return id;  }
            uint128_t secondary_key() const { return supply.symbol.code().raw().to128bits(); }
        };
        typedef eosio::multi_index<"stat"_n, item_asset,
            indexed_by<"second"_n, const_mem_fun<item_asset, uint128_t, &item_asset::secondary_key>>
        > stats;


        // for apps -------------
        TABLE item_spec { // scope: self
            uint64_t          id;
            int         maxgroup; // MAX (max quantity in same slot), 1 (no agroup), 0 (no limit)
            name            nick; 
            uint64_t      author; // table vapaeeaouthor::authors.id
            uint64_t primary_key() const { return id;  }
            uint128_t secondary_key() const { return boardbagebox::combine(nick, author); }
        };
        typedef eosio::multi_index<"itemspec"_n, item_spec,
            indexed_by<"second"_n, const_mem_fun<item_spec, uint128_t, &item_spec::secondary_key>>
        > item_specs;

        // Mastery ------------------------------------------------
        TABLE mastery_spec { // scope: self
            uint64_t          id;
            name            nick; 
            uint64_t      author; // table vapaeeaouthor::authors.id
            uint64_t primary_key() const { return id; }
            uint128_t secondary_key() const { return boardbagebox::combine(nick, author); }
        };
        typedef eosio::multi_index<"mastery"_n, mastery_spec,
            indexed_by<"second"_n, const_mem_fun<mastery_spec, uint128_t, &mastery_spec::secondary_key>>
        > mastery_specs;        

        TABLE mastery_prop { // scope: mastery_id
            uint64_t          id;
            slug            nick; 
            uint64_t primary_key() const { return id;  }
            uint128_t secondary_key() const { return nick.value.to128bits(); }
        };
        typedef eosio::multi_index<"masteryprop"_n, mastery_prop,
            indexed_by<"second"_n, const_mem_fun<mastery_prop, uint64_t, &mastery_prop::secondary_key>>
        > mastery_props;


        

        // ------------------- ITEM ------------------

        


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