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

#include "vapaeetokens.dispatcher.hpp"
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
        ACTION create( name issuer, asset maximum_supply) {
            PRINT("\nACTION vapaeetokens.create()\n");
            vapaee::token::core c;
            c.action_create_token(issuer, maximum_supply);
        };

        ACTION addissuer( name app, const symbol_code& symbol ) {
            PRINT("\nACTION vapaeetokens.addissuer()\n");
            vapaee::token::core c;
            c.action_add_token_issuer(app, symbol);
        };
        
        ACTION removeissuer( name app, const symbol_code& symbol ) {
            PRINT("\nACTION vapaeetokens.removeissuer()\n");
            vapaee::token::core c;
            c.action_remove_token_issuer(app, symbol);
        };        

        ACTION issue( name to, const asset& quantity, string memo ) {
            PRINT("\nACTION vapaeetokens.issue()\n");
            vapaee::token::core c;
            c.action_issue(to, quantity, memo);
        };

        ACTION burn(name owner, asset quantity, string memo ) {
            PRINT("\nACTION vapaeetokens.burn()\n");
            vapaee::token::core c;
            c.action_burn(owner, quantity, memo);
        };

        ACTION transfer(name from, name to, asset quantity, string  memo ) {
            PRINT("\nACTION vapaeetokens.transfer()\n");
            vapaee::token::core c;
            c.action_transfer(from, to, quantity, memo);
        };

        ACTION open( name owner, const symbol& symbol, name ram_payer ) {
            PRINT("\nACTION vapaeetokens.open()\n");
            vapaee::token::core c;
            c.action_open(owner, symbol, ram_payer);
        };

        ACTION close( name owner, const symbol& symbol ) {
            PRINT("\nACTION vapaeetokens.close()\n");
            vapaee::token::core c;
            c.action_close(owner, symbol);
        };


    public:
        // AIRDROP-ACTOINS  ------------------------------------------------------------------------------------------------------
        
        ACTION setsnapshot (name contract, uint64_t scope, const symbol_code& sym_code, int64_t cap, int64_t min, int64_t ratio, int64_t base, const std::string & memo) {
            PRINT("\nACTION vapaeetokens.setsnapshot()\n");
            vapaee::token::airdrop a;
            a.action_setsnapshot(contract, scope, sym_code, cap, min, ratio, base, memo);
        };

        /*ACTION nosnapshot (const symbol_code& symbolcode) {
            PRINT("\nACTION vapaeetokens.nosnapshot()\n");
            vapaee::token::airdrop a;
            a.action_nosnapshot(symbolcode);
        };*/

        ACTION claim (name owner, const symbol_code & symbolcode, name ram_payer) {
            PRINT("\nACTION vapaeetokens.claim()\n");
            vapaee::token::airdrop a;
            a.action_claim(owner, symbolcode, ram_payer);
        };


    public:
        // EXCHANGE-ACTOINS  ------------------------------------------------------------------------------------------------------
        ACTION addtoken (name contract, const symbol_code & symbol, uint8_t precision, name ram_payer) {
            PRINT("\nACTION vapaeetokens.addtoken()\n");
            vapaee::token::exchange e;
            e.action_addtoken(contract, symbol, precision, ram_payer);
        };

        ACTION cancel(name owner, name type, const symbol_code & token_a, const symbol_code & token_p, const std::vector<uint64_t> & orders) {
            PRINT("\nACTION vapaeetokens.cancel()\n");
            vapaee::token::exchange e;
            e.action_cancel(owner, type, token_a, token_p, orders);
        };

        ACTION retire(name owner, const asset & quantity) {
            PRINT("\nACTION vapaeetokens.retire()\n");
            vapaee::token::exchange e;
            e.action_retire(owner, quantity);
        };

        HANDLER htransfer(name from, name to, asset quantity, string  memo ) {
            PRINT("\nHANDLER vapaeetokens.htransfer()\n");

            // skipp handling outcoming transfers from this contract to outside
            if (from == get_self()) {
                print(from.to_string(), " to ", to.to_string(), ": ", quantity.to_string(), " vapaee::token::exchange::handler_transfer() skip...\n");
                return;
            }
            
            string order_str;
            int i,j,s;

            for (i=0,j=0,s=0; i<memo.size(); i++,j++) {
                if (memo[i] == '|') {
                    break;
                } else {
                    order_str += memo[i];
                }
            }

            // name order_type(order_str);

            if (order_str == string("sell") || order_str == string("buy") || order_str == string("deposit")) {
                vapaee::token::exchange e(get_code());
                e.handler_transfer(from, to, quantity, memo);                
            }
        }
        
        /*
        ACTION cancel2(name owner, name type, const symbol_code & token_a, const symbol_code & token_p, const std::vector<uint64_t> & orders) {
            PRINT("\nACTION vapaeetokens.cancel2()\n");
            vapaee::token::exchange e;
            e.action_cancel(owner, type, token_a, token_p, orders);
        };
        //*/
        
        ACTION dotick (name caller) {
            PRINT("\nACTION vapaeetokens.dotick()\n");
            require_auth( caller );
            vapaee::token::exchange e;
            e.aux_try_to_unlock(caller);
        };
        

        ACTION configfee(name action, const asset & fee) {
            PRINT("\nACTION vapaeetokens.configfee()\n");
            vapaee::token::exchange e;
            e.action_configfee(action, fee);
        };        

    public:
        // STAKE-ACTOINS  ------------------------------------------------------------------------------------------------------
        ACTION stake (name owner, const asset & quantity, name to, name concept) {
            PRINT("\nACTION vapaeetokens.stake()\n");
            vapaee::token::stake s;
            s.action_stake(owner, quantity, to, concept);
        };

        ACTION unstake (name owner, const asset & quantity, name from, name concept) {
            PRINT("\nACTION vapaeetokens.unstake()\n");
            vapaee::token::stake s;
            s.action_unstake(owner, quantity, from, concept);
        };

        ACTION restake (name owner, uint64_t id, name to, name concept) {
            PRINT("\nACTION vapaeetokens.restake()\n");
            vapaee::token::stake s;
            s.action_restake(owner, id, to, concept);
        };

        ACTION unstakeback (name owner) {
            PRINT("\nACTION vapaeetokens.unstakeback()\n");
            vapaee::token::stake s;
            s.action_unstakeback(owner);
        };

        ACTION unstaketime (name owner, const symbol_code & sym_code, uint64_t min_time, uint64_t max_time, uint64_t auto_stake) {
            PRINT("\nACTION vapaeetokens.unstaketime()\n");
            vapaee::token::stake s;
            s.action_unstaketime(owner, sym_code, min_time, max_time, auto_stake);
        };

};

}; // namespace