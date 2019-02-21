#pragma once
#include <vapaee/token/common.hpp>

#include <ctype.h>
#include <stdlib.h>

using namespace eosio;
namespace vapaee {
    namespace token {

    class exchange {
        name _self;
        name _code;
    
    public:
        
        exchange():_self(vapaee::token::contract),_code(vapaee::token::contract){}
        exchange(name code):_self(vapaee::token::contract),_code(code){}

        inline name get_self() const { return _self; }
        inline name get_code() const { return _code; }

        string aux_to_lowercase(string str) {
            string result = str;
            for (int i=0; i<str.length(); i++) {
                result[i] = tolower(str[i]);
            }
            return result;
        }
        
        void action_addtoken(name contract, const symbol_code & sym_code, uint8_t precision, name ram_payer) {
            PRINT("vapaee::token::exchange::action_addtoken()\n");
            PRINT(" contract: ", contract.to_string(), "\n");
            PRINT(" sym_code: ", sym_code.to_string(), "\n");
            PRINT(" precision: ", std::to_string((unsigned) precision), "\n");
            PRINT(" ram_payer: ", ram_payer.to_string(), "\n");

            require_auth(ram_payer);

            tokens tokenstable(get_self(), get_self().value);
            auto itr = tokenstable.find(sym_code.raw());
            eosio_assert(itr == tokenstable.end(), "Token already registered");
            tokenstable.emplace( ram_payer, [&]( auto& a ){
                a.contract = contract;
                a.symbol = sym_code;
                a.precision = precision;
            });
            PRINT("vapaee::token::exchange::action_addtoken() ...\n");
        }

        void handler_transfer(name from, name to, asset quantity, string memo) {
            // skipp handling outcoming transfers from this contract to outside
            if (from == get_self()) {
                print(from.to_string(), " to ", to.to_string(), ": ", quantity.to_string(), " vapaee::token::exchange::handler_transfer() skip...\n");
                return;
            }
            
            PRINT("vapaee::token::exchange::handler_transfer()\n");
            PRINT(" from: ", from.to_string(), "\n");
            PRINT(" to: ", to.to_string(), "\n");
            PRINT(" quantity: ", quantity.to_string(), "\n");
            PRINT(" memo: ", memo.c_str(), "\n");
            PRINT(" code: ", get_code(), "\n");


            string order_str;
            string quantity_str;
            string price_str;
            vector<string> strings = {""};
            PRINT(" strings.size(): ", std::to_string(strings.size()), "\n");
            int i,j,s;

            for (i=0,j=0,s=0; i<memo.size(); i++,j++) {
                // PRINT(" memo[", std::to_string(i), "]: ", memo[i], " j: ", std::to_string(j), " s: ", std::to_string(s), "\n");
                if (memo[i] == '|') {
                    s++;
                    j=0;
                    strings.push_back(string(""));
                    // PRINT(" i: ", std::to_string(i), " j: ", std::to_string(j), " s: ", std::to_string(s), " continue\n");
                    continue;
                } else {
                    strings[s] += memo[i];
                    // PRINT(" strings[", std::to_string(s), "][", std::to_string(j), "]: ", memo[i], " strings[", std::to_string(s), "]:", strings[s].c_str(), "\n");
                    // PRINT(" strings[", std::to_string(s), "]:", strings[s].c_str(), "\n");
                }
            }

            // PRINT(" strings[0]: ", strings[0].c_str(), "\n");
            // PRINT(" strings[1]: ", strings[1].c_str(), "\n");
            // PRINT(" strings[2]: ", strings[2].c_str(), "\n");

            eosio_assert(2==s, "memo malformed must be: 'order|amount|price'");
            name order_type(strings[0]);
            asset order_amount = vapaee::utils::string_to_asset(strings[1]);
            asset order_price = vapaee::utils::string_to_asset(strings[2]);
            asset order_quantity = quantity;
            asset payment = order_price;
            uint64_t price_unit = pow(10.0, order_price.symbol.precision());
            payment.amount = (int64_t)(order_amount.amount * order_price.amount / price_unit);

            PRINT(" order_type: ", order_type.to_string(), "\n");
            PRINT(" order_amount: ", order_amount.to_string(), "\n");
            PRINT(" order_price: ", order_price.to_string(), "\n");
            PRINT(" order_quantity: ", order_quantity.to_string(), "\n");
            
            eosio_assert(order_type == "sell"_n || order_type == "buy"_n, (string("order must be 'sell' or 'buy'. got: ") + order_type.to_string()).c_str());
            if (order_type == "sell"_n) {
                eosio_assert(quantity == order_amount, (string("in sell order quantity ") + quantity.to_string() + " is not the same as order_amount " + order_amount.to_string()).c_str());
            }
            if (order_type == "buy"_n) {
                eosio_assert(quantity == payment, (string("in buy order quantity ") + quantity.to_string() + " is not the same as payment " + payment.to_string()).c_str());
            }
            

            generate_order(from, order_type, order_amount, order_price, order_quantity);

            PRINT("vapaee::token::exchange::handler_transfer() ...\n");
        }

        void generate_order(name owner, name type, asset amount, asset price, asset quantity) {
            PRINT("vapaee::token::exchange::generate_order()\n");
            PRINT(" owner: ", owner.to_string(), "\n");
            PRINT(" type: ", type.to_string(), "\n");
            PRINT(" amount: ", amount.to_string(), "\n");
            PRINT(" price: ", price.to_string(), "\n");
            PRINT(" quantity: ", quantity.to_string(), "\n");

            require_auth(owner);            

            // create scope for the orders table
            string p_sym_str = price.symbol.code().to_string();
            string a_sym_str = amount.symbol.code().to_string();
            string scope_buy_str = a_sym_str + "." + p_sym_str;
            string scope_sell_str = p_sym_str + "." + a_sym_str;
            scope_buy_str = aux_to_lowercase(scope_buy_str); 
            scope_sell_str = aux_to_lowercase(scope_sell_str); 
            name scope_buy(scope_buy_str);
            name scope_sell(scope_sell_str);

            PRINT(" scope_buy: ", scope_buy.to_string(), "\n");
            PRINT(" scope_sell: ", scope_sell.to_string(), "\n");
            
            if (type == "sell"_n) {
                generate_sell_order(owner, scope_sell, scope_buy, amount, price, quantity);
            } else if (type == "buy"_n) {
                asset inverse = vapaee::utils::inverse(price, amount.symbol);
                generate_sell_order(owner, scope_buy, scope_sell, quantity, inverse, amount);
            } else {
                eosio_assert(false, (string("type must be 'sell' or 'buy' in lower case, got: ") + type.to_string()).c_str());
            }
            
            PRINT("vapaee::token::exchange::generate_order() ...\n");
        }

        void generate_sell_order(name owner, name scope_buy, name scope_sell, asset amount, asset price, asset quantity) {
            PRINT("vapaee::token::exchange::generate_sell_order()\n");
            PRINT(" owner: ", owner.to_string(), "\n");
            PRINT(" scope_buy: ", scope_buy.to_string(), "\n");
            PRINT(" scope_sell: ", scope_sell.to_string(), "\n");
            PRINT(" amount: ", amount.to_string(), "\n");
            PRINT(" price: ", price.to_string(), "\n");
            PRINT(" quantity: ", quantity.to_string(), "\n");
            
            sellorders buytable(get_self(), scope_buy.value);
            sellorders selltable(get_self(), scope_sell.value);
            // sellorders selltable(get_self(), scope.value);
            auto buy_index = buytable.template get_index<"price"_n>();
            // auto sell_index = selltable.template get_index<"price"_n>();
            auto sell_index = selltable.template get_index<"price"_n>();
            asset remaining = amount;
            asset payment = price;
            asset current_price;
            asset current_inverse;
            asset current_amount;
            asset inverse = vapaee::utils::inverse(price, amount.symbol);

            tokens tokenstable(get_self(), get_self().value);
            auto atk_itr = tokenstable.find(amount.symbol.code().raw());
            auto ptk_itr = tokenstable.find(price.symbol.code().raw());
            eosio_assert(atk_itr != tokenstable.end(), (string("Token ") + amount.symbol.code().to_string() + " not registered registered").c_str());
            eosio_assert(ptk_itr != tokenstable.end(), (string("Token ") + price.symbol.code().to_string() + " not registered registered").c_str());
            eosio_assert(atk_itr->precision == amount.symbol.precision(), aux_error_1(amount, atk_itr->precision).c_str());
            eosio_assert(ptk_itr->precision == price.symbol.precision(), aux_error_1(price, ptk_itr->precision).c_str());
            eosio_assert(quantity.symbol == price.symbol || quantity.symbol == amount.symbol , (string("quantity token symbol ") + price.symbol.code().to_string() + " is different from amount and price").c_str());

            uint64_t amount_unit = pow(10.0, atk_itr->precision);
            uint64_t price_unit = pow(10.0, ptk_itr->precision);            

            // iterate over a list or buy order from the maximun price down
            for (auto b_ptr = buy_index.begin(); b_ptr != buy_index.end(); b_ptr = buy_index.begin()) {
                eosio_assert(b_ptr->price.symbol == inverse.symbol, "buy order price symbol and inverse symbol are different");
                PRINT("  buyorder - price:", b_ptr->price.to_string(), " amount: ", b_ptr->amount.to_string(), " deposit: ", b_ptr->deposit.to_string(),"\n");
                PRINT("           inverse:", inverse.to_string() ,"\n");
                
                if (b_ptr->price.amount <= inverse.amount) {
                    // transaction !!!
                    current_price = b_ptr->price;   // TLOS
                    PRINT("   b_ptr->amount: ", b_ptr->amount.to_string(), " > remaining: ", remaining.to_string(),"\n");
                    if (b_ptr->amount > remaining) { // CNT
                        // buyer wants more that the user is selling -> reduces buyer amount
                        current_amount = remaining;  // CNT
                        payment.amount = (int64_t)( (double)(remaining.amount * b_ptr->deposit.amount) / (double)b_ptr->amount.amount);
                        buytable.modify(*b_ptr, same_payer, [&](auto& a){
                            a.amount -= remaining;   // CNT
                            a.deposit -= payment;    // TLOS
                        });
                        PRINT("   payment(1):  ", payment.to_string(),"\n");
                    } else {
                        // buyer gets all amount wanted -> destroy order
                        current_amount = b_ptr->amount;
                        payment = b_ptr->deposit;
                        buytable.erase(*b_ptr);
                        PRINT("   payment(2):  ", payment.to_string(),"\n");
                    }

                    // transfer to buyer CNT
                    remaining -= current_amount;
                    action(
                        permission_level{get_self(),"active"_n},
                        atk_itr->contract,
                        "transfer"_n,
                        std::make_tuple(get_self(), b_ptr->owner, current_amount, string(""))
                    ).send();
                    PRINT("   transfer ", current_amount.to_string(), " to ", b_ptr->owner.to_string(),"\n");
                        
                    // transfer to seller TLOS
                    action(
                        permission_level{get_self(),"active"_n},
                        ptk_itr->contract,
                        "transfer"_n,
                        std::make_tuple(get_self(), owner, payment, string(""))
                    ).send();
                    PRINT("   transfer ", payment.to_string(), " to ", owner.to_string(),"\n");
                    
                } else {
                    break;
                }

                PRINT("  remaining:", remaining.to_string(),"\n");
                if (remaining.amount <= 0) break;
            }

            if (remaining.amount > 0) {
                PRINT("  final remaining: ", remaining.to_string(), "\n");
                // insert sell order
                uint64_t id = selltable.available_primary_key();
                // inverse = vapaee::utils::inverse(price, amount.symbol);
                payment.amount = (int64_t)((double)(remaining.amount * price.amount) / (double)amount_unit);
                selltable.emplace( get_self(), [&]( auto& a ) {
                    a.id = id;
                    a.owner = owner;
                    a.price = price;       // CNT
                    a.amount = payment;    // TLOS 
                    a.deposit = remaining; // CNT
                });
                PRINT("  sellorders.emplace(): ", std::to_string((unsigned long long) id), "\n");
            }            
        }

        void action_cancel(name owner, name type, const symbol_code & token_a, const symbol_code & token_p, const std::vector<uint64_t> & orders) {
            PRINT("vapaee::token::exchange::action_cancel()\n");
            PRINT(" owner: ", owner.to_string(), "\n");
            PRINT(" type: ", type.to_string(), "\n");
            PRINT(" token_a: ",  token_a.to_string(), "\n");
            PRINT(" token_p: ",  token_p.to_string(), "\n");
            PRINT(" orders.size(): ", std::to_string((int) orders.size()), "\n");

            require_auth(owner);

            // create scope for the orders table
            string a_sym_str = token_a.to_string();
            string p_sym_str = token_p.to_string();
            string scope_buy_str = a_sym_str + "." + p_sym_str;
            string scope_sell_str = p_sym_str + "." + a_sym_str;
            scope_buy_str = aux_to_lowercase(scope_buy_str); 
            scope_sell_str = aux_to_lowercase(scope_sell_str); 
            name scope_buy(scope_buy_str);
            name scope_sell(scope_sell_str);

            if (type == "sell"_n) {
                cancel_sell_order(owner, scope_buy, token_a, orders);
            } 

            if (type == "buy"_n) {
                cancel_sell_order(owner, scope_sell, token_p, orders);
            }

            PRINT("vapaee::token::exchange::action_cancel() ...\n");
        }

        void cancel_sell_order(name owner, name scope, const symbol_code & token_p, const std::vector<uint64_t> & orders) {
            PRINT("vapaee::token::exchange::cancel_sell_order()\n");
            PRINT(" owner: ", owner.to_string(), "\n");
            PRINT(" scope: ", scope.to_string(), "\n");
            PRINT(" orders.size(): ", orders.size(), "\n");

            tokens tokenstable(get_self(), get_self().value);
            auto ptk_itr = tokenstable.find(token_p.raw());
            eosio_assert(ptk_itr != tokenstable.end(), (string("Token ") + token_p.to_string() + " not registered registered").c_str());            

            sellorders selltable(get_self(), scope.value);
            asset return_amount;

            for (int i=0; i<orders.size(); i++) {
                auto itr = selltable.find(orders[i]);
                eosio_assert(itr != selltable.end(), "buy order not found");
                eosio_assert(itr->owner == owner, "attemp to delete someone elses buy order");
                return_amount = itr->deposit;
                selltable.erase(itr);

                action(
                    permission_level{get_self(),"active"_n},
                    ptk_itr->contract,
                    "transfer"_n,
                    std::make_tuple(get_self(), owner, return_amount, string(""))
                ).send();
            }
        }


        string aux_error_1(const asset & amount, uint8_t precision) {
            return string("Token ") +
                amount.symbol.code().to_string() +
                " has not the same precision(" +
                std::to_string(amount.symbol.precision()) +
                ") as registered(" +
                std::to_string(precision) + 
                ")";
        }        

    }; // class

}; // namespace

}; // namespace