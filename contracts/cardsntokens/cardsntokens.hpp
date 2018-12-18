#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <vapaee/slug_asset.hpp>

using namespace eosio;
namespace vapaee {

#define template_type uint64_t

CONTRACT cardsntokens : public eosio::contract {
    public:
        using contract::contract;

        // --------------------- CARDS ------------------
        // envelop (es un item que tiene asociado un container)

        // -- lista de las ediciones de una carta (que en su gran mayoría va a ser una sola)
        // scope: card_post
        // row: representa una edición con supply copias de la carta definida en boardgamebox.item_asset
        TABLE edition {
            uint64_t id;
            name    name;
            uint64_t  author;
            slug_asset supply;
            string data;
            string preview;
            string preload;
            uint64_t deploy; // template 
            uint64_t front; // template
            uint64_t primary_key() const { return id; }
        };
        typedef eosio::multi_index<"edition"_n, edition> editions;
        
        // lista de todos los templates que tiene el sistema
        // scope: contract
        // row: representa un template 
        TABLE jsontemplate {
            uint64_t id;
            uint64_t  author;
            int    type; // deploy | front (es necesario? no podría ser incluido en el file.json?)
            name   name;
            string file;
            uint64_t primary_key() const { return id; }
        };
        typedef eosio::multi_index<"templat"_n, jsontemplate> templates;

        // lista de todos los templates que tiene el sistema
        // scope: contract
        // row: representa un template 
        TABLE card_post {
            uint64_t      id;
            uint64_t    item;
            int         type; // collection, numered, joker, item, aura, etc...
            uint64_t  author;
            uint64_t edition;
            int editions; // cuantas editions tiene
            uint64_t primary_key() const { return id; }
            uint64_t secondary_key() const { return item; } // y si uuso el item como primary?
        };
        typedef eosio::multi_index<"card"_n, card_post> cards;        
        // --------------------- ALBUMS ------------------
        // lista de todos los albums creados por algu autor
        // scope: contract
        // row: la definición de un album
        TABLE album_spec {
            uint64_t id; // 
            uint64_t container; // boardgamebox.container_spec
            uint64_t  author;
            // deploy_template
            // front_template
            // capacity
            // slots / pages
            uint64_t primary_key() const { return id; }
        };
        typedef eosio::multi_index<"album"_n, album_spec> albums;

        // lista de todas las colecciones para el usuario
        // scope: user
        // row: una collección de cartas asociada a un container_instance (donde están los item_units)
        TABLE collection_instance {
            uint64_t id; // 
            uint64_t container; // boardgamebox.container_instance

            // 
            uint64_t primary_key() const { return id; }
        };
        typedef eosio::multi_index<"collection"_n, collection_instance> collections;


};


}; // namespace