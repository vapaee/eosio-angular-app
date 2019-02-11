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
            print("\nACTION vapaee::token::exchange::action_addtoken()\n");
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
            print("vapaee::token::exchange::action_addtoken() ...\n");
            
        }

        void action_order(name owner, name type, asset amount, asset price) {
            print("\nACTION vapaee::token::exchange::action_order()\n");
            print(" owner: ", owner.to_string(), "\n");
            print(" type: ", type.to_string(), "\n");
            print(" amount: ", amount.to_string(), "\n");
            print(" price: ", price.to_string(), "\n");


            require_auth(owner);

            tokens tokenstable(get_self(), get_self().value);
            auto atk_itr = tokenstable.find(amount.symbol.code().raw());
            auto ptk_itr = tokenstable.find(price.symbol.code().raw());
            eosio_assert(atk_itr != tokenstable.end(), (string("Token ") + amount.symbol.code().to_string() + " not registered registered").c_str());
            eosio_assert(ptk_itr != tokenstable.end(), (string("Token ") + price.symbol.code().to_string() + " not registered registered").c_str());

            string p_sym_str = price.symbol.code().to_string();
            string a_sym_str = amount.symbol.code().to_string();
            string scope_str = a_sym_str + "." + p_sym_str;
            std::transform(scope_str.begin(), scope_str.end(), scope_str.begin(), ::tolower);
            name scope(scope_str);

            buyorders buytable(get_self(), scope.value);
            sellorders selltable(get_self(), scope.value);
            auto buy_index = buytable.template get_index<"price">();
            auto sell_index = selltable.template get_index<"price">();
            asset remaining = amount;

            if (type == "sell"_n) {
                auto b_ptr = buy_index.upper_bound(0xFFull);
                for (;b_ptr != buy_index.end(); buy_index++) {
                    eosio_assert(b_ptr->price.symbol == price.symbol, "price symbol are different");
                    if (b_ptr->price.amount >= price.amount) {
                        // transaction !!!
                        // transfer to buyer
                    }

                    if (remaining.amount <= 0) break;
                }

                if (remaining.amount > 0) {
                    // insert in sell table
                    // transfer to contract
                }

            } else if (type == "buy"_n) {

            } else {
                eosio_assert(false, (string("type must be 'sell' or 'buy' in lower case, got: ") + type.to_string()).c_str());
            }




            /*  

            tokenstable.emplace( ram_payer, [&]( auto& a ){
                a.contract = contract;
                a.symbol = symbol;
            });
            */
            print("vapaee::token::exchange::action_order() ...\n");
        }

        void action_cancel(name owner, name type, const std::vector<uint64_t> & orders) {

        }

    }; // class

}; // namespace

}; // namespace