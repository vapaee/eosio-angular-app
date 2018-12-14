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

        ACTION create(name   issuer,
                    slug_asset  maximum_supply);

        ACTION issue(name to, asset quantity, string memo);

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

    private:
        TABLE account {
            slug_asset    balance;
            uint64_t primary_key() const { print("primary_key() showld NOT be used on 'accounts' table.");  return 0;  }
            slug get_slug_key() const { return balance.symbol.code().raw(); }
        };

        TABLE currency_stats {
            slug_asset    supply;
            slug_asset    max_supply;
            name     issuer;
            uint64_t primary_key() const { print("primary_key() showld NOT be used on 'stat' table.");  return 0;  }
            slug get_slug_key() const { return supply.symbol.code().raw(); }
        };

        typedef eosio::multi_index<"accounts"_n, account, 
            indexed_by<"byslug"_n, const_mem_fun<account, slug, &account::get_slug_key>>
        > accounts;

        typedef eosio::multi_index<"stat"_n, currency_stats, 
            indexed_by<"byslug2"_n, const_mem_fun<currency_stats, slug, &currency_stats::get_slug_key>>
        > stats;
};