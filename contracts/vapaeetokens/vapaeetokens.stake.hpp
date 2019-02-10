#pragma once
#include <vapaee/token/common.hpp>


using namespace eosio;
namespace vapaee {
    namespace token {

    class stake {
        name _self;
    
    public:
        
        stake():_self(vapaee::token::contract){}

        inline name get_self()const { return vapaee::token::contract; }

        void action_stake (name owner, const asset & quantity, name to) {
            print("vapaee::token::stake::action_stake");
            print(" owner: ", owner.to_string(), "\n");
            print(" quantity: ", quantity.to_string(), "\n");
            print(" to: ", to.to_string(), "\n");

            require_auth(owner);

            // entry on stakes table
            stakes stakes_table( get_self(), owner.value );
            auto stakes_index = stakes_table.template get_index<"secondary"_n>();
            auto stakes_itr = stakes_index.find(vapaee::utils::combine(quantity.symbol.code().raw(), to.value));
            uint64_t id = stakes_table.available_primary_key();
            if (stakes_itr == stakes_index.end()) {
                stakes_table.emplace( owner, [&]( auto& a ){
                    a.id = id;
                    a.quantity = quantity;
                    a.since = current_time();
                    a.last = a.since;
                });        
            } else {
                stakes_table.modify( *stakes_itr, same_payer, [&]( auto& a ) {
                    a.quantity.amount += quantity.amount;
                    a.last = current_time();
                    id = a.id;
                });
            }

            // transfer that quantity to _self
            action(
                permission_level{owner,"active"_n},
                get_self(),// TODO: estoy asumiendo que el contrato del symbolo es vapaeetokens
                "transfer"_n,
                std::make_tuple(owner, get_self(), quantity, string("stake:") + std::to_string((int)id) + ",to:" + to.to_string())
            ).send();

            print("vapaee::token::stake::action_stake() ...\n");
        }

        void action_unstake (name owner, const asset & quantity, name from) {
            print("vapaee::token::stake::action_unstake");
            print(" owner: ", owner.to_string(), "\n");
            print(" quantity: ", quantity.to_string(), "\n");
            print(" from: ", from.to_string(), "\n");

            require_auth(owner);

            // entry on stakes table
            stakes stakes_table( get_self(), owner.value );
            auto stakes_index = stakes_table.template get_index<"secondary"_n>();
            auto stakes_itr = stakes_index.find(vapaee::utils::combine(quantity.symbol.code().raw(), from.value));

            // verificamos que exista un stake en nombre de from
            if (stakes_itr == stakes_index.end()) {
                string error1 = string("owner '") + owner.to_string() +
                    "' has not " +quantity.symbol.code().to_string() +
                    " tokens staked for '" + from.to_string() + "'";
                eosio_assert(false, error1.c_str() );
            }

            // verificamos que ese stake tenga más fondos de los que se solicita unstake
            if (stakes_itr->quantity.amount < quantity.amount) {
                string error2 = string("owner '") + owner.to_string() + 
                    "' has not enough " + quantity.symbol.code().to_string() + " tokens staked for '" + from.to_string() + "'";
                eosio_assert(false, error2.c_str() );
            }

            // si tiene la misma cantidad, hay que eliminar la entrada de la tabla
            if (stakes_itr->quantity.amount == quantity.amount) {
                stakes_table.erase( *stakes_itr );
            } else {
                // si tiene menos cantidad, hay que restarla de la entrada existente
                stakes_table.modify( *stakes_itr, same_payer, [&]( auto& a ) {
                    eosio_assert(a.quantity.amount > quantity.amount, "a.quantity.amount < quantity.amount");
                    a.quantity.amount -= quantity.amount;
                    a.last = current_time();
                });
            }

            unstakes unstakes_table( get_self(), owner.value );
            uint64_t id = unstakes_table.available_primary_key();
            unstakes_table.emplace( owner, [&]( auto& a ){
                a.id = id;
                a.quantity = quantity;
                a.block = current_time(); // TODO hay que agregar un tiempo basado en config.min_time y config.max_time
            });

            print("vapaee::token::stake::action_unstake() ...\n");
        }

        void action_restake (name owner, uint64_t unstake_id, name to) {
            print("vapaee::token::stake::action_restake");
            print(" owner: ", owner.to_string(), "\n");
            print(" unstake_id: ", std::to_string((int) unstake_id), "\n");
            print(" to: ", to.to_string(), "\n");

            require_auth(owner);

            unstakes unstakes_table( get_self(), owner.value );
            auto unstake_itr = unstakes_table.get(unstake_id);
            asset quantity = unstake_itr.quantity;
            unstakes_table.erase(unstake_itr);

            // entry on stakes table
            stakes stakes_table( get_self(), owner.value );
            auto stakes_index = stakes_table.template get_index<"secondary"_n>();
            auto stakes_itr = stakes_index.find(vapaee::utils::combine(quantity.symbol.code().raw(), to.value));
            uint64_t id = stakes_table.available_primary_key();
            if (stakes_itr == stakes_index.end()) {
                stakes_table.emplace( owner, [&]( auto& a ){
                    a.id = id;
                    a.quantity = quantity;
                    a.since = current_time();
                    a.last = a.since;
                });        
            } else {
                stakes_table.modify( *stakes_itr, same_payer, [&]( auto& a ) {
                    a.quantity.amount += quantity.amount;
                    a.last = current_time();
                    id = a.id;
                });
            }    

            print("vapaee::token::stake::action_restake() ...\n");
        }

        void action_unstakeback (name owner) {
            print("vapaee::token::stake::action_unstakeback");
            print(" owner: ", owner.to_string(), "\n");

            unstakes table( get_self(), owner.value );
            auto index = table.template get_index<"block"_n>();
            uint64_t now = current_time();
            auto itr = index.upper_bound(now);    

            if (itr != index.end() ) {
                // erase the unstakes entry
                asset quantity = itr->quantity;
                table.erase(*itr);

                // return fouds to the owner
                action(
                    permission_level{owner,"active"_n},
                    get_self(),// TODO: estoy asumiendo que el contrato del symbolo es vapaeetokens
                    "transfer"_n,
                    std::make_tuple(get_self(), owner, quantity, string("unstaking ") + quantity.to_string())
                ).send();
            }
            print("vapaee::token::stake::action_unstakeback() ...\n");
        }

        void action_unstaketime (name owner, const symbol_code & sym_code, uint64_t min_time, uint64_t max_time, uint64_t auto_stake) {
            print("vapaee::token::stake::action_unstaketime");
            print(" owner: ", owner.to_string(), "\n");
            print(" sym_code: ", sym_code.to_string(), "\n");
            print(" min_time: ", std::to_string((int) min_time), "\n");
            print(" max_time: ", std::to_string((int) max_time), "\n");
            print(" auto_stake: ", std::to_string((int) auto_stake), "\n");

            config table( get_self(), owner.value );
            auto itr = table.find( sym_code.raw() );
            if( itr == table.end() ) {
                table.emplace( owner, [&]( auto& a ){
                    a.sym_code = sym_code;
                    a.min_time = min_time;
                    a.max_time = max_time;
                    a.auto_stake = auto_stake;
                });
            } else {
                table.modify( *itr, same_payer, [&]( auto& a ) {
                    a.min_time = min_time;
                    a.max_time = max_time;
                    a.auto_stake = auto_stake;
                });
            }
            print("vapaee::token::stake::action_unstaketime() ...\n");
        }        
        
    }; // class

}; // namespace

}; // namespace