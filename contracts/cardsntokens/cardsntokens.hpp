#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;

CONTRACT hello : public eosio::contract {
  public:
      using contract::contract;

      ACTION hi( name user );

      // accessor for external contracts to easily send inline actions to your contract
      using hi_action = action_wrapper<"hi"_n, &hello::hi>;


      /*
      
        -- CNT Cards --
        accounts: TABLE cnt_account
        cards: TABLE card
        editions: TABLE edition

        -- CNT Albums --
        accounts: TABLE cnt_account
        stats: TABLE slug_stats


    

      */
};