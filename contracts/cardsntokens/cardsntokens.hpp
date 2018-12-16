#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <vapaee/slug_asset.hpp>

using namespace eosio;
using namespace vapaee;

#define template_type uint64_t

CONTRACT cardsntokens : public eosio::contract {
    public:
        using contract::contract;

        /*
        -- CNT Cards --
        accounts: TABLE cnt_account
        cards: TABLE card
        editions: TABLE edition

        -- CNT Albums --
        accounts: TABLE cnt_account
        stats: TABLE slug_stats
        */


        // --------------------- CARDS ------------------
        TABLE edition {
            uint64_t id;
            name   name;
            string data;
            string preview;
            string preload;
            template_type deploy;
            template_type front;
            uint64_t primary_key() const { return id; }
        };
        
        TABLE jsontemplate {
            template_type id;
            int type; // deploy | front
            name   name;
            string file;
            uint64_t primary_key() const { return id; }
        };

        typedef eosio::multi_index<"edition"_n, edition> editions;
        typedef eosio::multi_index<"templat"_n, jsontemplate> templates;
        // --------------------- ALBUMS ------------------


        // --------------------- PUBLISHERS ------------------


};