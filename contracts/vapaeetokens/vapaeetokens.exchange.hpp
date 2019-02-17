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
            print("vapaee::token::exchange::action_addtoken()\n");
            print(" contract: ", contract.to_string(), "\n");
            print(" sym_code: ", sym_code.to_string(), "\n");
            print(" precision: ", std::to_string((unsigned) precision), "\n");
            print(" ram_payer: ", ram_payer.to_string(), "\n");

            require_auth(ram_payer);

            tokens tokenstable(get_self(), get_self().value);
            auto itr = tokenstable.find(sym_code.raw());
            eosio_assert(itr == tokenstable.end(), "Token already registered");
            tokenstable.emplace( ram_payer, [&]( auto& a ){
                a.contract = contract;
                a.symbol = sym_code;
                a.precision = precision;
            });
            print("vapaee::token::exchange::action_addtoken() ...\n");
        }

        void handler_transfer(name from, name to, asset quantity, string memo) {
            // skipp handling outcoming transfers from this contract to outside
            if (from == get_self()) {
                print(from.to_string(), " to ", to.to_string(), ": ", quantity.to_string(), " vapaee::token::exchange::handler_transfer() skip...\n");
                return;
            }
            
            print("vapaee::token::exchange::handler_transfer()\n");
            print(" from: ", from.to_string(), "\n");
            print(" to: ", to.to_string(), "\n");
            print(" quantity: ", quantity.to_string(), "\n");
            print(" memo: ", memo.c_str(), "\n");
            print(" code: ", get_code(), "\n");


            string order_str;
            string quantity_str;
            string price_str;
            vector<string> strings = {"","",""};
            print(" strings.size(): ", std::to_string(strings.size()), "\n");
            int i,j,s;

            for (i=0,j=0,s=0; i<memo.size(); i++,j++) {
                // print(" memo[", std::to_string(i), "]: ", memo[i], " j: ", std::to_string(j), " s: ", std::to_string(s), "\n");
                if (memo[i] == '|') {
                    s++;
                    j=0;
                    // print(" i: ", std::to_string(i), " j: ", std::to_string(j), " s: ", std::to_string(s), " continue\n");
                    continue;
                } else {
                    strings[s] += memo[i];
                    // print(" strings[", std::to_string(s), "][", std::to_string(j), "]: ", memo[i], " strings[", std::to_string(s), "]:", strings[s].c_str(), "\n");
                    // print(" strings[", std::to_string(s), "]:", strings[s].c_str(), "\n");
                }
            }

            // print(" strings[0]: ", strings[0].c_str(), "\n");
            // print(" strings[1]: ", strings[1].c_str(), "\n");
            // print(" strings[2]: ", strings[2].c_str(), "\n");

            eosio_assert(2==s, "memo malformed must be: 'order|amount|price'");
            name order_type(strings[0]);
            asset order_amount = vapaee::utils::string_to_asset(strings[1]);
            asset order_price = vapaee::utils::string_to_asset(strings[2]);
            asset order_quantity = quantity;
            asset payment = order_price;
            uint64_t price_unit = pow(10.0, order_price.symbol.precision());
            payment.amount = (int64_t)(order_amount.amount * order_price.amount / price_unit);

            print(" order_type: ", order_type.to_string(), "\n");
            print(" order_amount: ", order_amount.to_string(), "\n");
            print(" order_price: ", order_price.to_string(), "\n");
            print(" order_quantity: ", order_quantity.to_string(), "\n");

            
            eosio_assert(order_type == "sell"_n || order_type == "buy"_n, (string("order must be 'sell' or 'buy'. got: ") + order_type.to_string()).c_str());
            if (order_type == "sell"_n) {
                eosio_assert(quantity == order_amount, (string("in sell order quantity ") + quantity.to_string() + " is not the same as order_amount " + order_amount.to_string()).c_str());
            }
            if (order_type == "buy"_n) {
                eosio_assert(quantity == payment, (string("in buy order quantity ") + quantity.to_string() + " is not the same as payment " + payment.to_string()).c_str());
            }
            

            generate_order(from, order_type, order_amount, order_price, order_quantity);

            print("vapaee::token::exchange::handler_transfer() ...\n");
        }

        string get_error_1(const asset & amount, uint8_t precision) {
            return string("Token ") +
                amount.symbol.code().to_string() +
                " has not the same precision(" +
                std::to_string(amount.symbol.precision()) +
                ") as registered(" +
                std::to_string(precision) + 
                ")";
        }

        void generate_order(name owner, name type, asset amount, asset price, asset quantity) {
            print("apaee::token::exchange::generate_order()\n");
            print(" owner: ", owner.to_string(), "\n");
            print(" type: ", type.to_string(), "\n");
            print(" amount: ", amount.to_string(), "\n");
            print(" price: ", price.to_string(), "\n");
            print(" quantity: ", quantity.to_string(), "\n");

            require_auth(owner);

            tokens tokenstable(get_self(), get_self().value);
            print(" tokens tokenstable()\n");
            auto atk_itr = tokenstable.find(amount.symbol.code().raw());
            auto ptk_itr = tokenstable.find(price.symbol.code().raw());
            print(" tokenstable.find()\n");
            eosio_assert(atk_itr != tokenstable.end(), (string("Token ") + amount.symbol.code().to_string() + " not registered registered").c_str());
            eosio_assert(ptk_itr != tokenstable.end(), (string("Token ") + price.symbol.code().to_string() + " not registered registered").c_str());
            eosio_assert(atk_itr->precision == amount.symbol.precision(), get_error_1(amount, atk_itr->precision).c_str());
            eosio_assert(ptk_itr->precision == price.symbol.precision(), get_error_1(price, ptk_itr->precision).c_str());
            eosio_assert(quantity.symbol == price.symbol || quantity.symbol == amount.symbol , (string("quantity token symbol ") + price.symbol.code().to_string() + " is different from amount and price").c_str());
            print(" eosio_assert() OK!\n");

            // create scope for the orders table
            string p_sym_str = price.symbol.code().to_string();
            string a_sym_str = amount.symbol.code().to_string();
            string scope_str = a_sym_str + "." + p_sym_str;
            scope_str = aux_to_lowercase(scope_str); 
            name scope(scope_str);
            print(" scope: ", scope.to_string(), "\n");

            buyorders buytable(get_self(), scope.value);
            sellorders selltable(get_self(), scope.value);
            auto buy_index = buytable.template get_index<"price"_n>();
            auto sell_index = selltable.template get_index<"price"_n>();
            asset remaining = amount;
            asset current_deposit = quantity;
            asset payment = price;
            asset current_price;
            asset current_amount;

            uint64_t amount_unit = pow(10.0, atk_itr->precision);
            uint64_t price_unit = pow(10.0, ptk_itr->precision);
           
            
            if (type == "sell"_n) {
                // iterate over a list or buy order from the maximun price down
                for (auto b_ptr = buy_index.begin(); b_ptr != buy_index.end(); b_ptr = buy_index.begin()) {
                    eosio_assert(b_ptr->price.symbol == price.symbol, "price symbol are different");
                    print("  buyorder - price:", b_ptr->price.to_string(), " amount: ", b_ptr->amount.to_string(),"\n");
                    if (b_ptr->price.amount >= price.amount) {
                        // transaction !!!
                        current_price = b_ptr->price;

                        if (b_ptr->amount > remaining) {
                            // buyer wants more that the user is selling -> reduces buyer amount
                            current_amount = remaining;
                            payment.amount = (int64_t)(current_amount.amount * current_price.amount / price_unit);
                            buytable.modify(*b_ptr, same_payer, [&](auto& a){
                                a.amount -= remaining;
                                a.deposit -= payment;
                            });
                        } else {
                            // buyer gets all amount wanted -> destroy order
                            current_amount = b_ptr->amount;
                            payment = b_ptr->deposit;
                            buytable.erase(*b_ptr);
                        }

                        // transfer to buyer
                        remaining -= current_amount;
                        action(
                            permission_level{get_self(),"active"_n},
                            atk_itr->contract,
                            "transfer"_n,
                            std::make_tuple(get_self(), b_ptr->owner, current_amount, string(""))
                        ).send();
                        
                        // transfer to seller
                        action(
                            permission_level{get_self(),"active"_n},
                            ptk_itr->contract,
                            "transfer"_n,
                            std::make_tuple(get_self(), owner, payment, string(""))
                        ).send();
                        
                        
                    } else {
                        break;
                    }

                    print("  remaining:", remaining.to_string(),"\n");
                    if (remaining.amount <= 0) break;
                }

                if (remaining.amount > 0) {
                    print("  final remaining: ", remaining.to_string(), "\n");
                    // insert sell order
                    uint64_t id = selltable.available_primary_key();
                    selltable.emplace( get_self(), [&]( auto& a ) {
                        a.id = id;
                        a.owner = owner;
                        a.amount = remaining;
                        a.price = price;
                        a.deposit = remaining;
                    });
                    print("  sellorders.emplace(): ", std::to_string((unsigned long long) id), "\n");
                }

            } else if (type == "buy"_n) {
                
                // iterate over a list or sell order from the bottom price (0) up
                for (auto s_ptr = sell_index.begin(); s_ptr != sell_index.end(); s_ptr = sell_index.begin()) {
                    eosio_assert(s_ptr->price.symbol == price.symbol, "price symbol are different");
                    print("  sellorder - price:", s_ptr->price.to_string(), " amount: ", s_ptr->amount.to_string(),"\n");
                    if (s_ptr->price.amount <= price.amount) {
                        // transaction !!!
                        current_price = s_ptr->price;

                        if (s_ptr->amount > remaining) {
                            // seller wants to sell more that the user is buying -> reduces seller amount
                            print("  sellorders.modify(): ", std::to_string((int)s_ptr->id), "-", amount.to_string(), "\n");
                            selltable.modify(*s_ptr, same_payer, [&](auto& a){
                                a.amount -= remaining;
                            });
                            current_amount = remaining;
                        } else {
                            // seller sells all -> destroy order
                            print("  sellorders.erase(): ", std::to_string((int)s_ptr->id), "\n");
                            selltable.erase(*s_ptr);
                            current_amount = s_ptr->amount;
                        }

                        // transfer to buyer
                        remaining.amount -= current_amount.amount;
                        print("  remaining: ", remaining.to_string(), "\n");
                        action(
                            permission_level{get_self(),"active"_n},
                            atk_itr->contract,
                            "transfer"_n,
                            std::make_tuple(get_self(), owner, current_amount, string(""))
                        ).send();
                        
                        // transfer to seller
                        payment.amount = (int64_t)(current_amount.amount * current_price.amount / price_unit);
                        action(
                            permission_level{get_self(),"active"_n},
                            ptk_itr->contract,
                            "transfer"_n,
                            std::make_tuple(get_self(), s_ptr->owner, payment, string(""))
                        ).send();                        
                    } else {
                        break;
                    }

                    print("  remaining:", remaining.to_string(),"\n");
                    if (remaining.amount <= 0) break;
                }

                if (remaining.amount > 0) {
                    print("  final remaining: ", remaining.to_string(), "\n");
                    // insert buy order
                    uint64_t id = buytable.available_primary_key();
                    buytable.emplace( get_self(), [&]( auto& a ) {
                        a.id = id;
                        a.owner = owner;
                        a.price = price;
                        a.amount = remaining;
                        a.deposit = quantity;
                    }); 
                    

                    print("  buyorders.emplace(): ", std::to_string((unsigned long long) id), "\n");
                }

            } else {
                eosio_assert(false, (string("type must be 'sell' or 'buy' in lower case, got: ") + type.to_string()).c_str());
            }

            
            print("vapaee::token::exchange::generate_order() ...\n");
        }

        void action_cancel(name owner, name type, const symbol_code & token_a, const symbol_code & token_p, const std::vector<uint64_t> & orders) {
            print("vapaee::token::exchange::action_cancel()\n");
            print(" owner: ", owner.to_string(), "\n");
            print(" type: ", type.to_string(), "\n");
            print(" token_a: ",  token_a.to_string(), "\n");
            print(" token_p: ",  token_p.to_string(), "\n");
            print(" orders.size(): ", std::to_string((int) orders.size()), "\n");

            tokens tokenstable(get_self(), get_self().value);
            auto atk_itr = tokenstable.find(token_a.raw());
            auto ptk_itr = tokenstable.find(token_p.raw());
            eosio_assert(atk_itr != tokenstable.end(), (string("Token ") + token_a.to_string() + " not registered registered").c_str());
            eosio_assert(ptk_itr != tokenstable.end(), (string("Token ") + token_p.to_string() + " not registered registered").c_str());


            string a_sym_str = token_a.to_string();
            string p_sym_str = token_p.to_string();
            string scope_str = a_sym_str + "." + p_sym_str;
            scope_str = aux_to_lowercase(scope_str);
            name scope(scope_str);
            print("  scope: ", scope.to_string(), "\n");

            buyorders buytable(get_self(), scope.value);
            sellorders selltable(get_self(), scope.value);
            asset return_amount;

            if (type == "sell"_n) {
                for (int i=0; i<orders.size(); i++) {
                    auto itr = selltable.find(orders[i]);
                    eosio_assert(itr != selltable.end(), "sell order not found");
                    eosio_assert(itr->owner == owner, "attemp to delete someone elses sell order");
                    return_amount = itr->deposit;
                    selltable.erase(itr);

                    action(
                        permission_level{get_self(),"active"_n},
                        atk_itr->contract,
                        "transfer"_n,
                        std::make_tuple(get_self(), owner, return_amount, string(""))
                    ).send();                     
                }
            }

            if (type == "buy"_n) {
                for (int i=0; i<orders.size(); i++) {
                    auto itr = buytable.find(orders[i]);
                    eosio_assert(itr != buytable.end(), "buy order not found");
                    eosio_assert(itr->owner == owner, "attemp to delete someone elses buy order");
                    return_amount = itr->deposit;
                    buytable.erase(itr);

                    action(
                        permission_level{get_self(),"active"_n},
                        ptk_itr->contract,
                        "transfer"_n,
                        std::make_tuple(get_self(), owner, return_amount, string(""))
                    ).send();
                }
            }

            print("vapaee::token::exchange::action_cancel() ...\n");
        }

    }; // class

}; // namespace

}; // namespace