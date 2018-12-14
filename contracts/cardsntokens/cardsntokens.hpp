#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <vapaee/slug_asset.hpp>

using namespace eosio;
using namespace vapaee;

CONTRACT cardsntokens : public eosio::contract {
    public:
        using contract::contract;

        ACTION hi( name user );

        // accessor for external contracts to easily send inline actions to your contract
        using hi_action = action_wrapper<"hi"_n, &cardsntokens::hi>;

        uint64_t a;

        /*
        
          -- CNT Cards --
          accounts: TABLE cnt_account
          cards: TABLE card
          editions: TABLE edition

          -- CNT Albums --
          accounts: TABLE cnt_account
          stats: TABLE slug_stats
        */

         

         struct [[eosio::table]] account {
            slug_asset    balance;
            uint64_t primary_key() const { print("primary_key() showld NOT be used on 'accounts' table.");  return 0;  }
            slug get_slug_key() const { return balance.symbol.code().raw(); }
         };

         struct [[eosio::table]] currency_stats {
            slug_asset    supply;
            slug_asset    max_supply;
            name     issuer;

            uint64_t primary_key() const { print("primary_key() showld NOT be used on 'stat' table.");  return 0;  }
            slug get_slug_key() const { return supply.symbol.code().raw(); }
         };

         // typedef eosio::multi_index< "accounts"_n, account > accounts;
         // typedef eosio::multi_index< "stat"_n, currency_stats > stats;


         typedef eosio::multi_index<"accounts"_n, account, 
            indexed_by<"byslug"_n, const_mem_fun<account, slug, &account::get_slug_key>>
         > accounts;

         typedef eosio::multi_index<"stat"_n, currency_stats, 
            indexed_by<"byslug2"_n, const_mem_fun<currency_stats, slug, &currency_stats::get_slug_key>>
         > stats;
};