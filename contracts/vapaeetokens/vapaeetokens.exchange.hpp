#pragma once
#include <vapaee/token/common.hpp>


using namespace eosio;
namespace vapaee {
    namespace token {

    class exchange {
        name _self;
    
    public:
        
        exchange():_self(vapaee::token::contract){}

        inline name get_self()const { return vapaee::token::contract; }
        
        void action_addtoken(name contract, const symbol_code & symbol, name ram_payer) {
            print("\nACTION vapaee::token::exchange::addtoken()\n");
            print(" contract: ", contract.to_string(), "\n");
            print(" symbol: ", symbol.to_string(), "\n");
            print(" ram_payer: ", ram_payer.to_string(), "\n");

            require_auth(ram_payer);

            tokens tokenstable(get_self(), get_self().value);
            auto itr = tokenstable.find(symbol.raw());
            eosio_assert(itr == tokenstable.end(), "Token already registered");
            tokenstable.emplace( ram_payer, [&]( auto& a ){
                a.contract = contract;
                a.symbol = symbol;
            });
            print("vapaee::token::exchange::addtoken() ...\n");
        }        
    }; // class

}; // namespace

}; // namespace