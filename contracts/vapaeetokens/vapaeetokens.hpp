#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>

#include <string>

using namespace eosio;
using namespace std;


namespace vapaee {

CONTRACT vapaeetokens : public eosio::contract {
    public:


        // vapaeetokens(): cnt_token() {}

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

       // TOKEN ------------------------------------------------------------------------------------------------------

    private:
        TABLE account {
            asset    balance;

            uint64_t primary_key()const { return balance.symbol.code().raw(); }
        };

        TABLE currency_stats {
            asset    supply;
            asset    max_supply;
            name     issuer;

            uint64_t primary_key()const { return supply.symbol.code().raw(); }
        };

        typedef eosio::multi_index< "accounts"_n, account > accounts;
        typedef eosio::multi_index< "stat"_n, currency_stats > stats;

        void sub_balance( name owner, asset value );
        void add_balance( name owner, asset value, name ram_payer );

    public:
        using contract::contract;

        ACTION create( name   issuer,
                    asset  maximum_supply);

        ACTION issue( name to, asset quantity, string memo );

        ACTION retire( asset quantity, string memo );

        ACTION transfer( name    from,
                    name    to,
                    asset   quantity,
                    string  memo );

        ACTION open( name owner, const symbol& symbol, name ram_payer );

        ACTION close( name owner, const symbol& symbol );

        static asset get_supply( name token_contract_account, symbol_code sym_code )
        {
            stats statstable( token_contract_account, sym_code.raw() );
            const auto& st = statstable.get( sym_code.raw() );
            return st.supply;
        }

        static asset get_balance( name token_contract_account, name owner, symbol_code sym_code )
        {
            accounts accountstable( token_contract_account, owner.value );
            const auto& ac = accountstable.get( sym_code.raw() );
            return ac.balance;
        }

       // TOKEN ------------------------------------------------------------------------------------------------------


};

}; // namespace