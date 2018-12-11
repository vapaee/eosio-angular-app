#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;

CONTRACT vapaeetokens : public eosio::contract {
  public:
      using contract::contract;

      ACTION hi( name user );

      // accessor for external contracts to easily send inline actions to your contract
      using hi_action = action_wrapper<"hi"_n, &vapaeetokens::hi>;

      /*
        -- VPE Accounts --
        publishers: TABLE publisher <-- (prefix)

        -- VPE Tokens --
        accounts: TABLE account
        stats: TABLE vpe_tokens

        -- VPE Market --
        buy_orders: token_order
        sell_order: token_order

      */
};