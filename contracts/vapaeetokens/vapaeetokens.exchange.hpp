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

        void aux_substract_deposits(name owner, const asset & amount, bool erase, name ram_payer) {
            PRINT("vapaee::token::exchange::aux_substract_deposits()\n");
            PRINT(" owner: ", owner.to_string(), "\n");
            PRINT(" amount: ", amount.to_string(), "\n");
            PRINT(" erase: ", std::to_string(erase), "\n");

            deposits depositstable(get_self(), owner.value);
            auto itr = depositstable.find(amount.symbol.code().raw());
            eosio_assert(itr != depositstable.end(), "user has no deposits for this token to subtract from");
            if (itr->amount == amount) {
                PRINT("  itr->amount == amount: ",  amount.to_string(), "\n");
                if (erase) {
                    depositstable.erase(itr);
                }
            } else {
                PRINT("  itr->amount > amount: ", itr->amount.to_string(), " > ", amount.to_string(),  "\n");
                eosio_assert(itr->amount > amount, "user has not enought deposits to subtract from");
                depositstable.modify(*itr, aux_get_modify_payer(ram_payer), [&](auto& a){
                    a.amount -= amount;
                });
            }
            PRINT("vapaee::token::exchange::aux_substract_deposits() ...\n");
        }

        name aux_get_modify_payer(name ram_payer) {
            return (ram_payer == get_self()) ? same_payer : ram_payer;
        }

        void aux_add_deposits(name owner, const asset & amount, name ram_payer) {
            PRINT("vapaee::token::exchange::aux_add_deposits()\n");
            PRINT(" owner: ", owner.to_string(), "\n");
            PRINT(" amount: ", amount.to_string(), "\n");
            PRINT(" ram_payer: ", ram_payer.to_string(), "\n");

            deposits depositstable(get_self(), owner.value);
            auto itr = depositstable.find(amount.symbol.code().raw());
            if (itr == depositstable.end()) {
                depositstable.emplace( ram_payer, [&]( auto& a ){
                    a.amount = amount;
                });
            } else {
                depositstable.modify(*itr, aux_get_modify_payer(ram_payer), [&](auto& a){
                    a.amount += amount;
                });
            }
            PRINT("vapaee::token::exchange::aux_add_deposits() ...\n");            
        }        

        void aux_load_lock(uint64_t id, locks_table & lock) {
            locks lockstable(get_self(), get_self().value);
            auto itr = lockstable.find(id);
            eosio_assert(itr != lockstable.end(), "no lock with this id was found");
            lock.id = itr->id;
            lock.owner = itr->owner;
            lock.amount = itr->amount;
            lock.expire = itr->expire;
            lock.concept = itr->concept;
            lock.foreign = itr->foreign;
            lock.scope = itr->scope;
        }

        uint64_t aux_create_lock(name owner, const asset & amount, uint64_t expire, name concept, uint64_t foreign, uint64_t scope, name ram_payer) {
            PRINT("vapaee::token::exchange::aux_create_lock()\n");
            PRINT(" owner: ", owner.to_string(), "\n");
            PRINT(" amount: ", amount.to_string(), "\n");
            PRINT(" expire: ", std::to_string((unsigned) expire), "\n");
            PRINT(" concept: ", concept.to_string(), "\n");
            PRINT(" foreign: ", std::to_string((unsigned) foreign), "\n");
            PRINT(" scope: ", std::to_string((unsigned) scope), "\n");
            PRINT(" ram_payer: ", ram_payer.to_string(), "\n");

            aux_substract_deposits(owner, amount, false, ram_payer);
            locks lockstable(get_self(), get_self().value);
            uint64_t id = lockstable.available_primary_key();

            PRINT("  lockstable.emplace(): ", std::to_string((unsigned long long) id), "\n");
            lockstable.emplace( ram_payer, [&]( auto& a ){
                a.id = id;
                a.owner = owner;
                a.amount = amount;
                a.expire = expire;
                a.concept = concept;
                a.foreign = foreign;
                a.scope = scope;
            });
            

            PRINT("vapaee::token::exchange::aux_create_lock() ...\n");
            return id;
        }

        void aux_cancel_lock(uint64_t id, name owner, name ram_payer) {
            PRINT("vapaee::token::exchange::aux_cancel_lock()\n");
            PRINT(" id: ", std::to_string((unsigned) id), "\n");
            PRINT(" owner: ", owner.to_string(), "\n");
            PRINT(" ram_payer: ", ram_payer.to_string(), "\n");

            locks lockstable(get_self(), get_self().value);
            auto itr = lockstable.find(id);
            eosio_assert(itr != lockstable.end(), "no lock with this id was found");
            eosio_assert(owner != itr->owner, "lock owner is different from caller owner");

            // load lock data (amount)
            locks_table lock;
            aux_load_lock(id, lock);

            // erase lock entry
            lockstable.erase(*itr);
            
            // deposit back the users tokens
            aux_add_deposits(owner, lock.amount, ram_payer);

            PRINT("vapaee::token::exchange::aux_cancel_lock() ...\n");  
        }

        void aux_charge_fee_for_order(uint64_t lock_id, name owner, const asset & amount, const asset & payment, name ram_payer) {
            PRINT("vapaee::token::exchange::aux_charge_fee_for_order()\n");
            PRINT(" lock_id: ", std::to_string((unsigned long) lock_id), "\n");
            PRINT(" owner: ", owner.to_string(), "\n");
            PRINT(" amount: ", amount.to_string(), "\n");
            PRINT(" payment: ", payment.to_string(), "\n");
            
            locks lockstable(get_self(), get_self().value);
            auto lock_itr = lockstable.find(lock_id);
            eosio_assert(lock_itr != lockstable.end(), "locked amount is not th same symbol as fee config");
            symbol_code code = lock_itr->amount.symbol.code();
            PRINT("  code: ", code.to_string(), "\n");

            feeconfig feetable(get_self(), get_self().value);
            auto fee_itr = feetable.find(code.raw());
            eosio_assert(fee_itr != feetable.end(), "locked amount is not th same symbol as fee config");
            
            asset total_fee = lock_itr->amount;
            asset tlos;
            if (amount.symbol.code().to_string() == string("TLOS")) {
                tlos = amount;
            }
            if (payment.symbol.code().to_string() == string("TLOS")) {
                tlos = payment;
            }

            total_fee.amount = tlos.amount * ((double)fee_itr->fee.amount / (double)pow(10.0, fee_itr->fee.symbol.precision()));

            if (total_fee == lock_itr->amount) {
                PRINT("  locks.erase(): ", std::to_string((unsigned long)lock_itr->id), "\n");
                lockstable.erase(*lock_itr);
            } else {
                PRINT("  total_fee: ", total_fee.to_string(),"  lock_itr->amount: ", lock_itr->amount.to_string(), "\n");
                eosio_assert(total_fee < lock_itr->amount, "total_fee.amount is more than lock_itr->amount");
                lockstable.modify(*lock_itr, aux_get_modify_payer(ram_payer), [&](auto& a){
                    a.amount -= total_fee;
                });

                PRINT("  locks.modify(): ", std::to_string((unsigned long)lock_itr->id), " amount: ", lock_itr->amount.to_string(),"\n");
            }

            aux_add_deposits(get_self(), total_fee, ram_payer);

            PRINT("vapaee::token::exchange::aux_charge_fee_for_order() ...\n");            
        }

        void aux_charge_fee_for_sell(name owner, const asset & amount, const asset & payment, name ram_payer) {
            PRINT("vapaee::token::exchange::aux_charge_fee_for_sell()\n");
            PRINT(" owner: ", owner.to_string(), "\n");
            PRINT(" amount: ", amount.to_string(), "\n");
            PRINT(" payment: ", payment.to_string(), "\n");

            asset total_fee;
            aux_alculate_total_fee(owner, amount, payment, total_fee);

            aux_substract_deposits(owner, total_fee, true, ram_payer);
            aux_add_deposits(get_self(), total_fee, ram_payer);

            PRINT("vapaee::token::exchange::aux_charge_fee_for_sell() ...\n");            
        }

        void aux_alculate_total_fee(name owner, const asset & amount_a, const asset & amount_b, asset & total_fee) {
            PRINT("vapaee::token::exchange::aux_alculate_total_fee()\n");
            PRINT(" owner: ", owner.to_string(), "\n");
            PRINT(" amount_a: ", amount_a.to_string(), "\n");
            PRINT(" amount_b: ", amount_b.to_string(), "\n");

            feeconfig feetable(get_self(), get_self().value);
            deposits depositstable(get_self(), owner.value);

            asset tlos;
            if (amount_a.symbol.code().to_string() == string("TLOS")) {
                tlos = amount_a;
            }
            if (amount_b.symbol.code().to_string() == string("TLOS")) {
                tlos = amount_b;
            }

            auto itr = feetable.begin();
            bool success = false;
            PRINT("  tlos: ", tlos.to_string(), "\n");
            if (tlos.amount > 0) {
                for (itr = feetable.begin(); itr != feetable.end(); itr++) {
                    auto deposit_itr = depositstable.find(itr->fee.symbol.code().raw());
                    PRINT("- fee: ", itr->fee.to_string(), "\n");
                    PRINT("  deposit: ", deposit_itr->amount.to_string(), "\n");
                    if (deposit_itr != depositstable.end() ) {
                        total_fee = itr->fee;
                        total_fee.amount = tlos.amount * ((double)itr->fee.amount / (double)pow(10.0, itr->fee.symbol.precision()));
                        PRINT("  total_fee: ", total_fee.to_string(), "\n");
                        if (deposit_itr->amount > total_fee) {
                            // user has enough fee-token to pay the fee -> we lock them up
                            success = true;
                            PRINT("  success !! \n");
                            break;
                        }
                    }
                }
            } else {
                // no fees because no tlos involved
                success = true;
            }

            eosio_assert(success, "user don't have enough deposists to pay the order fee");

            PRINT(" -> total_fee: ", total_fee.to_string(), "\n");
            PRINT("vapaee::token::exchange::aux_alculate_total_fee() ...\n");
        }

        uint64_t aux_lock_fee_for_order(name scope_sell, const sell_order_table & sell_order, name concept, name ram_payer) {
            PRINT("vapaee::token::exchange::aux_lock_fee_for_order()\n");
            PRINT(" scope_sell: ", scope_sell.to_string(), "\n");
            PRINT(" sell_order.price: ", sell_order.price.to_string(), "\n");
            PRINT(" sell_order.amount: ", sell_order.amount.to_string(), "\n");
            PRINT(" sell_order.deposit: ", sell_order.deposit.to_string(), "\n");
            PRINT(" sell_order.amount.symbol.code().to_string(): ", sell_order.amount.symbol.code().to_string(), "\n");
            PRINT(" concept: ", concept.to_string(), "\n");
         
            name owner = sell_order.owner;
            asset total_fee;
            aux_alculate_total_fee(owner, sell_order.amount, sell_order.deposit, total_fee);
            uint64_t id = aux_create_lock(owner, total_fee, eosio::asset::max_amount, concept, sell_order.id, scope_sell.value, ram_payer);

            PRINT("vapaee::token::exchange::aux_lock_fee_for_order() ...\n");
            return id;
        }

        void aux_try_to_unlock(name ram_payer) {
            PRINT("vapaee::token::exchange::aux_try_to_unlock()\n");
            PRINT(" ram_payer: ", ram_payer.to_string(), "\n");

            int max = 5;           
            uint64_t now = current_time();
            PRINT(" now: ", std::to_string((unsigned long long) now), "\n");

            locks lockstable(get_self(), get_self().value);
            auto index = lockstable.template get_index<"expire"_n>();
            auto itr = index.lower_bound(now);

            for (int count = max; itr != index.end() && count > 0; count--, itr = index.lower_bound(now)) {
                PRINT("  -- itr->expire: ", std::to_string((unsigned long long) itr->expire), "\n");
                if (itr->expire < now) {
                    aux_cancel_lock(itr->id, itr->owner, ram_payer);
                } else {
                    break;
                }
            }

            PRINT("vapaee::token::exchange::aux_try_to_unlock() ...\n");
        }

        void action_retire(name owner, const asset & quantity) {
            PRINT("vapaee::token::exchange::action_retire()\n");
            PRINT(" owner: ", owner.to_string(), "\n");
            PRINT(" quantity: ", quantity.to_string(), "\n");

            // substract or remove deposit entry
            require_auth(owner);
            aux_substract_deposits(owner, quantity, true, owner);

            // send tokens
            tokens tokenstable(get_self(), get_self().value);
            auto ptk_itr = tokenstable.find(quantity.symbol.code().raw());
            eosio_assert(ptk_itr != tokenstable.end(), (string("Token ") + quantity.symbol.code().to_string() + " not registered registered").c_str());

            action(
                permission_level{get_self(),"active"_n},
                ptk_itr->contract,
                "transfer"_n,
                std::make_tuple(get_self(), owner, quantity, string("retire deposits: ") + quantity.to_string())
            ).send();
            PRINT("   transfer ", quantity.to_string(), " to ", owner.to_string(),"\n");

            PRINT("vapaee::token::exchange::action_retire() ...\n");
        }

        void action_configfee(name action, const asset & fee) {
            PRINT("vapaee::token::exchange::action_configfee()\n");
            PRINT(" action: ", action.to_string(), "\n");
            PRINT(" fee: ", fee.to_string(), "\n");

            require_auth(get_self());

            feeconfig feetable(get_self(), get_self().value);
            auto itr = feetable.find(fee.symbol.code().raw());

            if (itr == feetable.end()) {
                feetable.emplace( get_self(), [&]( auto& a ){
                    a.fee = fee;
                });
                PRINT("  feeconfig.emplace() ", fee.to_string(), "\n");
            } else {
                if (action == "update"_n) {
                    PRINT("  feeconfig.modify() ", fee.to_string(), "\n");
                    feetable.modify(*itr, get_self(), [&](auto& a){
                        a.fee = fee;
                    });
                }
                if (action == "erase"_n) {
                    feetable.erase(*itr);
                    PRINT("  feeconfig.erase() ", fee.to_string(), "\n");
                }
            }

            PRINT("vapaee::token::exchange::action_configfee() ...\n");            
            
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
            if (to != get_self()) {
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
            name ram_payer = get_self();
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
            if (strings[0] == string("sell") || strings[0] == string("buy")) {
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
                aux_generate_order(from, order_type, order_amount, order_price, order_quantity, ram_payer);

            } else if (strings[0] == string("deposit")) {
                name receiver;
                if (strings.size() == 1) {
                    receiver = from;
                } else if (strings.size() == 2) {
                    receiver = name(strings[1]);
                }
                PRINT(" receiver: ", receiver.to_string(), "\n");
                require_recipient(receiver);
                PRINT(" ram_payer: ", ram_payer.to_string(), "\n");
                aux_add_deposits(receiver, quantity, get_self());
            }

            PRINT("vapaee::token::exchange::handler_transfer() ...\n");
        }

        void aux_generate_order(name owner, name type, asset amount, asset price, asset quantity, name ram_payer) {
            PRINT("vapaee::token::exchange::aux_generate_order()\n");
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
                aux_generate_sell_order(owner, scope_sell, scope_buy, amount, price, quantity, ram_payer);
            } else if (type == "buy"_n) {
                asset inverse = vapaee::utils::inverse(price, amount.symbol);
                aux_generate_sell_order(owner, scope_buy, scope_sell, quantity, inverse, amount, ram_payer);
            } else {
                eosio_assert(false, (string("type must be 'sell' or 'buy' in lower case, got: ") + type.to_string()).c_str());
            }
            
            PRINT("vapaee::token::exchange::aux_generate_order() ...\n");
        }

        void aux_generate_sell_order(name owner, name scope_buy, name scope_sell, asset amount, asset price, asset quantity, name ram_payer) {
            PRINT("vapaee::token::exchange::aux_generate_sell_order()\n");
            PRINT(" owner: ", owner.to_string(), "\n");
            PRINT(" scope_buy: ", scope_buy.to_string(), "\n");
            PRINT(" scope_sell: ", scope_sell.to_string(), "\n");
            PRINT(" amount: ", amount.to_string(), "\n");            // CNT
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
            sell_order_table order;

            locks lockstable(get_self(), get_self().value);
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
                        // buyer wants more that the user is selling -> reduces buyer order amount
                        current_amount = remaining;  // CNT
                        payment.amount = (int64_t)( (double)(remaining.amount * b_ptr->deposit.amount) / (double)b_ptr->amount.amount);
                        buytable.modify(*b_ptr, aux_get_modify_payer(ram_payer), [&](auto& a){
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
                    // order = selltable.get(b_ptr->id);

                    aux_charge_fee_for_order(b_ptr->lock, b_ptr->owner, current_amount, payment, ram_payer);
                    aux_charge_fee_for_sell(owner, current_amount, payment, ram_payer);

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
                //uint64_t next_lock = lockstable.available_primary_key();
                //PRINT("  next_lock: ", std::to_string((unsigned long long) next_lock), "\n");
                // inverse = vapaee::utils::inverse(price, amount.symbol);
                payment.amount = (int64_t)((double)(remaining.amount * price.amount) / (double)amount_unit);
                selltable.emplace( ram_payer, [&]( auto& a ) {
                    a.id = id;
                    a.owner = owner;
                    a.price = price;       // CNT
                    a.amount = payment;    // TLOS 
                    a.deposit = remaining; // CNT
                });
                PRINT("  sellorders.emplace(): ", std::to_string((unsigned long long) id), "\n");
                
                order = selltable.get(id);
                uint64_t lock_id = aux_lock_fee_for_order(scope_sell, order, "sellorderfee"_n, ram_payer);

                selltable.modify(selltable.find(id), aux_get_modify_payer(ram_payer), [&]( auto& a ) {
                    a.lock = lock_id;
                });
            }
            
            PRINT("vapaee::token::exchange::aux_generate_sell_order() ...\n");
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
            locks lockstable(get_self(), get_self().value);
            auto ptk_itr = tokenstable.find(token_p.raw());
            eosio_assert(ptk_itr != tokenstable.end(), (string("Token ") + token_p.to_string() + " not registered registered").c_str());            

            sellorders selltable(get_self(), scope.value);
            asset return_amount;
            uint64_t lock;

            for (int i=0; i<orders.size(); i++) {
                auto itr = selltable.find(orders[i]);
                eosio_assert(itr != selltable.end(), "buy order not found");
                eosio_assert(itr->owner == owner, "attemp to delete someone elses buy order");
                return_amount = itr->deposit;
                auto lock = lockstable.find(itr->lock);
                PRINT("- itr->lock: ", std::to_string((unsigned) itr->lock), "\n");
                PRINT("  itr->amount: ", itr->amount.to_string(), "\n");
                eosio_assert(lock != lockstable.end(), "lock reference does notexist");

                aux_add_deposits(owner, lock->amount, owner);

                selltable.erase(*itr);
                lockstable.erase(*lock);

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