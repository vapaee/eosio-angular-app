#pragma once
#include <vapaee/token/common.hpp>

#include <ctype.h>

using namespace eosio;
namespace vapaee {
    namespace token {

    class exchange {
        name _self;
    
    public:
        
        exchange():_self(vapaee::token::contract){}

        inline name get_self()const { return vapaee::token::contract; }

        string tolowercase(string str) {
            string result;
            for (int i=0; i<str.length(); i++) {
                result[i] = tolower(str[i]);
            }
            return result;
        }
        
        void action_addtoken(name contract, const symbol & symbol, name ram_payer) {
            print("\nACTION vapaee::token::exchange::action_addtoken()\n");
            print(" contract: ", contract.to_string(), "\n");
            print(" symbol: ", symbol.code().to_string(), "\n");
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
            scope_str = tolowercase(scope_str); 
            name scope(scope_str);
            print("  scope: ", scope.to_string(), "\n");

            buyorders buytable(get_self(), scope.value);
            sellorders selltable(get_self(), scope.value);
            auto buy_index = buytable.template get_index<"price"_n>();
            auto sell_index = selltable.template get_index<"price"_n>();
            asset remaining = amount;
            asset payment = price;
            asset current_price;
            asset current_amount;

            uint64_t amount_unit = pow(10.0, amount.symbol.precision());
            uint64_t price_unit = pow(10.0, price.symbol.precision());
           

            if (type == "sell"_n) {
                // iterate over a list or buy order from the maximun price down
                for (auto b_ptr = buy_index.upper_bound(asset::max_amount); b_ptr != buy_index.end(); b_ptr++) {
                    eosio_assert(b_ptr->price.symbol == price.symbol, "price symbol are different");
                    if (b_ptr->price.amount >= price.amount) {
                        // transaction !!!
                        current_price = b_ptr->price;

                        if (b_ptr->amount.amount > amount.amount) {
                            // buyer wants more that the user is selling -> reduces buyer amount
                            buytable.modify(*b_ptr, same_payer, [&](auto& a){
                                a.amount.amount -= amount.amount;
                            });
                            current_amount = amount;
                        } else {
                            // buyer gets all amount wanted -> destroy order
                            buytable.erase(*b_ptr);
                            current_amount = b_ptr->amount;
                        }

                        // transfer to buyer
                        remaining.amount -= current_amount.amount;
                        action(
                            permission_level{owner,"active"_n},
                            atk_itr->contract,
                            "transfer"_n,
                            std::make_tuple(owner, b_ptr->owner, current_amount, string(""))
                        ).send();
                        
                        // transfer to seller
                        payment.amount = (int64_t)(current_amount.amount * current_price.amount / price_unit);
                        action(
                            permission_level{get_self(),"active"_n},
                            ptk_itr->contract,
                            "transfer"_n,
                            std::make_tuple(get_self(), owner, payment, string(""))
                        ).send();                        
                    }

                    if (remaining.amount <= 0) break;
                }

                if (remaining.amount > 0) {
                    // insert sell order
                    selltable.emplace( owner, [&]( auto& a ) {
                        a.id = selltable.available_primary_key();
                        a.amount = remaining;
                        a.price = price;
                    });

                    // transfer to contract
                    action(
                        permission_level{owner,"active"_n},
                        atk_itr->contract,
                        "transfer"_n,
                        std::make_tuple(owner, get_self(), remaining, string(""))
                    ).send();
                }

            } else if (type == "buy"_n) {
                
                // iterate over a list or sell order from the bottom price (0) up
                for (auto s_ptr = sell_index.lower_bound(0); s_ptr != sell_index.end(); s_ptr++) {
                    eosio_assert(s_ptr->price.symbol == price.symbol, "price symbol are different");
                    if (s_ptr->price.amount <= price.amount) {
                        // transaction !!!
                        current_price = s_ptr->price;

                        if (s_ptr->amount.amount > amount.amount) {
                            // seller wants more that the user is selling -> reduces seller amount
                            selltable.modify(*s_ptr, same_payer, [&](auto& a){
                                a.amount.amount -= amount.amount;
                            });
                            current_amount = amount;
                        } else {
                            // seller sells all -> destroy order
                            selltable.erase(*s_ptr);
                            current_amount = s_ptr->amount;
                        }

                        // transfer to buyer
                        remaining.amount -= current_amount.amount;
                        action(
                            permission_level{get_self(),"active"_n},
                            atk_itr->contract,
                            "transfer"_n,
                            std::make_tuple(get_self(), owner, current_amount, string(""))
                        ).send();
                        
                        // transfer to seller
                        payment.amount = (int64_t)(current_amount.amount * current_price.amount / price_unit);
                        action(
                            permission_level{owner,"active"_n},
                            ptk_itr->contract,
                            "transfer"_n,
                            std::make_tuple(owner, s_ptr->owner, payment, string(""))
                        ).send();                        
                    }

                    if (remaining.amount <= 0) break;
                }

                if (remaining.amount > 0) {
                    // insert buy order
                    buytable.emplace( owner, [&]( auto& a ) {
                        a.id = buytable.available_primary_key();
                        a.amount = remaining;
                        a.price = price;
                    });

                    // transfer to contract
                    payment.amount = (int64_t)(remaining.amount * price.amount / price_unit);
                    action(
                        permission_level{owner,"active"_n},
                        ptk_itr->contract,
                        "transfer"_n,
                        std::make_tuple(owner, get_self(), payment, string(""))
                    ).send();
                }

            } else {
                eosio_assert(false, (string("type must be 'sell' or 'buy' in lower case, got: ") + type.to_string()).c_str());
            }

            print("vapaee::token::exchange::action_order() ...\n");
        }

        void action_cancel(name owner, name type, const symbol & token_a, const symbol & token_p, const std::vector<uint64_t> & orders) {
            print("\nACTION vapaee::token::exchange::action_cancel()\n");
            print(" owner: ", owner.to_string(), "\n");
            print(" type: ", type.to_string(), "\n");
            print(" token_a: ",  token_a.code().to_string(), "\n");
            print(" token_p: ",  token_p.code().to_string(), "\n");
            print(" orders.size(): ", std::to_string((int) orders.size()), "\n");

            tokens tokenstable(get_self(), get_self().value);
            auto atk_itr = tokenstable.find(token_a.code().raw());
            auto ptk_itr = tokenstable.find(token_p.code().raw());
            eosio_assert(atk_itr != tokenstable.end(), (string("Token ") + token_a.code().to_string() + " not registered registered").c_str());
            eosio_assert(ptk_itr != tokenstable.end(), (string("Token ") + token_p.code().to_string() + " not registered registered").c_str());


            string a_sym_str = token_a.code().to_string();
            string p_sym_str = token_p.code().to_string();
            string scope_str = a_sym_str + "." + p_sym_str;
            scope_str = tolowercase(scope_str);
            name scope(scope_str);
            print("  scope: ", scope.to_string(), "\n");

            buyorders buytable(get_self(), scope.value);
            sellorders selltable(get_self(), scope.value);
            asset return_amount;

            if (type == "sell"_n) {
                for (int i=0; i<orders.size(); i++) {
                    auto itr = selltable.get(orders[i], "sell order not found");
                    eosio_assert(itr.owner == owner, "attemp to delete someone elses sell order");
                    return_amount = itr.amount;
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
                uint64_t price_unit = pow(10.0, token_p.precision());

                for (int i=0; i<orders.size(); i++) {
                    auto itr = buytable.get(orders[i], "buy order not found");
                    eosio_assert(itr.owner == owner, "attemp to delete someone elses buy order");
                    return_amount.symbol = token_p;
                    return_amount.amount = (int64_t)(itr.price.amount * itr.price.amount / price_unit);
                    buytable.erase(itr);

                    action(
                        permission_level{get_self(),"active"_n},
                        atk_itr->contract,
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