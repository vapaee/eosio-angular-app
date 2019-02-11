#pragma once
#include <vapaee/token/common.hpp>

using namespace eosio;
using namespace std;

using namespace eosio;
namespace vapaee {
    namespace bgbox {
#include <vapaee/bgbox/tables/apps.hpp>
    };
};

#include "vapaeetokens.core.hpp"
#include "vapaeetokens.airdrop.hpp"
#include "vapaeetokens.exchange.hpp"
#include "vapaeetokens.stake.hpp"


namespace vapaee {

CONTRACT vapaeetokens : public eosio::contract {
    
    private:
#include <vapaee/token/tables.all.hpp>

    public:
       // TOKEN-ACTOINS ------------------------------------------------------------------------------------------------------

        using contract::contract;
        ACTION create( name issuer, asset maximum_supply, uint64_t app) {
            print("\nACTION vapaeetokens.create()\n");
            vapaee::token::core c;
            c.action_create_token(issuer, maximum_supply, app);
        };

        ACTION addissuer( uint64_t app, const symbol& symbol ) {
            print("\nACTION vapaeetokens.addissuer()\n");
            vapaee::token::core c;
            c.action_add_token_issuer(app, symbol);
        };
        
        ACTION removeissuer( uint64_t app, const symbol& symbol ) {
            print("\nACTION vapaeetokens.removeissuer()\n");
            vapaee::token::core c;
            c.action_remove_token_issuer(app, symbol);
        };        

        ACTION issue( name to, const asset& quantity, string memo ) {
            print("\nACTION vapaeetokens.issue()\n");
            vapaee::token::core c;
            c.action_issue(to, quantity, memo);
        };

        ACTION burn(name owner, asset quantity, string memo ) {
            print("\nACTION vapaeetokens.burn()\n");
            vapaee::token::core c;
            c.action_burn(owner, quantity, memo);
        };

        ACTION transfer(name from, name to, asset quantity, string  memo ) {
            print("\nACTION vapaeetokens.transfer()\n");
            vapaee::token::core c;
            c.action_transfer(from, to, quantity, memo);
        };

        ACTION open( name owner, const symbol& symbol, name ram_payer ) {
            print("\nACTION vapaeetokens.open()\n");
            vapaee::token::core c;
            c.action_open(owner, symbol, ram_payer);
        };

        ACTION close( name owner, const symbol& symbol ) {
            print("\nACTION vapaeetokens.close()\n");
            vapaee::token::core c;
            c.action_close(owner, symbol);
        };

        
    public:
        // AIRDROP-ACTOINS  ------------------------------------------------------------------------------------------------------
        
        ACTION setsnapshot (name contract, uint64_t scope, const symbol_code& symbolcode, int64_t cap, int64_t min, int64_t ratio, int64_t base) {
            print("\nACTION vapaeetokens.setsnapshot()\n");
            vapaee::token::airdrop a;
            a.action_setsnapshot(contract, scope, symbolcode, cap, min, ratio, base);
        };

        ACTION nosnapshot (const symbol_code& symbolcode) {
            print("\nACTION vapaeetokens.nosnapshot()\n");
            vapaee::token::airdrop a;
            a.action_nosnapshot(symbolcode);
        };

        ACTION claim (name owner, const symbol_code & symbolcode, name ram_payer) {
            print("\nACTION vapaeetokens.claim()\n");
            vapaee::token::airdrop a;
            a.action_claim(owner, symbolcode, ram_payer);
        };


    public:
        // EXCHANGE-ACTOINS  ------------------------------------------------------------------------------------------------------
        ACTION addtoken (name contract, const symbol_code & symbol, name ram_payer) {
            print("\nACTION vapaeetokens.addtoken()\n");
            vapaee::token::exchange e;
            e.action_addtoken(contract, symbol, ram_payer);
        };

        ACTION order(name owner, name type, asset amount, asset price) {
            print("\nACTION vapaeetokens.order()\n");
            vapaee::token::exchange e;
            e.action_order(owner, type, amount, price);
        };

        ACTION cancel(name owner, name type, const std::vector<uint64_t> & orders) {
            print("\nACTION vapaeetokens.cancel()\n");
            vapaee::token::exchange e;
            e.action_cancel(owner, type, orders);
        };



    public:
        // STAKE-ACTOINS  ------------------------------------------------------------------------------------------------------
        ACTION stake (name owner, const asset & quantity, name to) {
            print("\nACTION vapaeetokens.stake()\n");
            vapaee::token::stake s;
            s.action_stake(owner, quantity, to);
        };

        ACTION unstake (name owner, const asset & quantity, name from) {
            print("\nACTION vapaeetokens.unstake()\n");
            vapaee::token::stake s;
            s.action_unstake(owner, quantity, from);
        };

        ACTION restake (name owner, uint64_t id, name to) {
            print("\nACTION vapaeetokens.restake()\n");
            vapaee::token::stake s;
            s.action_restake(owner, id, to);
        };

        ACTION unstakeback (name owner) {
            print("\nACTION vapaeetokens.unstakeback()\n");
            vapaee::token::stake s;
            s.action_unstakeback(owner);
        };

        ACTION unstaketime (name owner, const symbol_code & sym_code, uint64_t min_time, uint64_t max_time, uint64_t auto_stake) {
            print("\nACTION vapaeetokens.unstaketime()\n");
            vapaee::token::stake s;
            s.action_unstaketime(owner, sym_code, min_time, max_time, auto_stake);
        };




};

}; // namespace