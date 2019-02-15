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
            print("vapaee::token::stake::action_stake()\n");
            print(" owner: ", owner.to_string(), "\n");
            print(" quantity: ", quantity.to_string(), "\n");
            print(" to: ", to.to_string(), "\n");
            print(" current_time(): ", std::to_string((unsigned long long) current_time() ), "\n");

            require_auth(owner);
            eosio_assert( is_account( to ), "to account does not exist");

            // entry on stakes table
            stakes stakes_table( get_self(), owner.value );
            auto stakes_index = stakes_table.template get_index<"secondary"_n>();
            auto stakes_itr = stakes_index.find(vapaee::utils::combine(quantity.symbol.code().raw(), to.value));
            uint64_t id = stakes_table.available_primary_key();
            if (stakes_itr == stakes_index.end()) {
                stakes_table.emplace( owner, [&]( auto& a ){
                    a.id = id;
                    a.to = to;
                    a.quantity = quantity;
                    a.since = current_time();
                    a.last = a.since;
                });
                print(" stakes.emplace() staking ", quantity.to_string(), " to ", to.to_string(), "\n");
            } else {
                asset total;
                stakes_table.modify( *stakes_itr, same_payer, [&]( auto& a ) {
                    a.quantity += quantity;
                    a.last = current_time();
                    // -----------
                    id = a.id;
                    total = a.quantity;
                });
                print(" stakes.modify() adding ", quantity.to_string(), " to ", to.to_string(), " for a total of ", total.to_string(),"\n");
            }

            tokens tokens_table(get_self(), get_self().value);
            auto tkn = tokens_table.find(quantity.symbol.code().raw());
            eosio_assert(tkn != tokens_table.end(), "token not registered");            

            // transfer that quantity to _self
            action(
                permission_level{owner,"active"_n},
                tkn->contract,
                "transfer"_n,
                std::make_tuple(owner, get_self(), quantity, std::to_string((unsigned long long)id) + " - staking " + quantity.to_string() + " to " + to.to_string())
            ).send();

            print("vapaee::token::stake::action_stake() ...\n");
        }

        void action_unstake (name owner, const asset & quantity, name from) {
            print("vapaee::token::stake::action_unstake()\n");
            print(" owner: ", owner.to_string(), "\n");
            print(" quantity: ", quantity.to_string(), "\n");
            print(" from: ", from.to_string(), "\n");
            print(" current_time(): ", std::to_string((unsigned long long) current_time() ), "\n");

            require_auth(owner);

            // entry on stakes table
            stakes stakes_table( get_self(), owner.value );
            auto stakes_index = stakes_table.template get_index<"secondary"_n>();
            auto stakes_itr = stakes_index.find(vapaee::utils::combine(quantity.symbol.code().raw(), from.value));

            // we verify that there is a stake in the name of "from"
            if (stakes_itr == stakes_index.end()) {
                string error1 = string("owner '") + owner.to_string() +
                    "' has not " +quantity.symbol.code().to_string() +
                    " tokens staked for '" + from.to_string() + "'";
                eosio_assert(false, error1.c_str() );
            }

            // We verify that this stake has the same or more funds that are requested to unstake
            if (stakes_itr->quantity.amount < quantity.amount) {
                string error2 = string("owner '") + owner.to_string() + 
                    "' has not enough " + quantity.symbol.code().to_string() + " tokens staked for '" + from.to_string() + "'";
                eosio_assert(false, error2.c_str() );
            }

            // if it has the same amount, you have to delete the entry from the table
            if (stakes_itr->quantity.amount == quantity.amount) {
                print(" stakes.erase() ", std::to_string((unsigned long long) stakes_itr->id), " - ", quantity.to_string(),"\n");
                stakes_table.erase( *stakes_itr );
                
            } else {
                // si tiene menos cantidad, hay que restarla de la entrada existente
                print(" stakes.modify() ", std::to_string((unsigned long long) stakes_itr->id), " - ", quantity.to_string(),"\n");
                stakes_table.modify( *stakes_itr, same_payer, [&]( auto& a ) {
                    eosio_assert(a.quantity.amount > quantity.amount, "a.quantity.amount < quantity.amount");
                    a.quantity.amount -= quantity.amount;
                    a.last = current_time();
                });
            }

            unstakes unstakes_table( get_self(), owner.value );
            uint64_t id = unstakes_table.available_primary_key();
            print(" unstakes.emplace() ", std::to_string((unsigned long long) id), " - ", quantity.to_string(),"\n");
            unstakes_table.emplace( owner, [&]( auto& a ){
                a.id = id;
                a.quantity = quantity;
                a.expire = current_time() + 120000000; // TODO hay que agregar un tiempo basado en config.min_time y config.max_time
                // a.expire = 1000000000000000;
            });

            print("vapaee::token::stake::action_unstake() ...\n");
        }

        void action_restake (name owner, uint64_t unstake_id, name to) {
            print("vapaee::token::stake::action_restake()\n");
            print(" owner: ", owner.to_string(), "\n");
            print(" unstake_id: ", std::to_string((unsigned long long) unstake_id), "\n");
            print(" to: ", to.to_string(), "\n");
            print(" current_time(): ", std::to_string((unsigned long long) current_time() ), "\n");

            require_auth(owner);
            eosio_assert( is_account( to ), "to account does not exist");

            // remove the unstake entry
            unstakes unstakes_table( get_self(), owner.value );
            auto unstake_itr = unstakes_table.find(unstake_id);
            asset quantity = unstake_itr->quantity;
            unstakes_table.erase(unstake_itr);

            // create entry on stakes table (no tokens transfering)
            stakes stakes_table( get_self(), owner.value );
            auto stakes_index = stakes_table.template get_index<"secondary"_n>();
            auto stakes_itr = stakes_index.find(vapaee::utils::combine(quantity.symbol.code().raw(), to.value));
            uint64_t id = stakes_table.available_primary_key();
            if (stakes_itr == stakes_index.end()) {
                stakes_table.emplace( owner, [&]( auto& a ){
                    a.id = id;
                    a.to = to;
                    a.quantity = quantity;
                    a.since = current_time();
                    a.last = a.since;
                });        
            } else {
                stakes_table.modify( *stakes_itr, same_payer, [&]( auto& a ) {
                    a.quantity += quantity;
                    a.last = current_time();
                    id = a.id;
                });
            }    

            print("vapaee::token::stake::action_restake() ...\n");
        }

        void action_unstakeback (name owner) {
            print("vapaee::token::stake::action_unstakeback()\n");
            print(" owner: ", owner.to_string(), "\n");
            print(" current_time(): ", std::to_string((unsigned long long) current_time() ), "\n");
            
            tokens tokens_table(get_self(), get_self().value);
            unstakes table( get_self(), owner.value );
            auto index = table.template get_index<"expire"_n>();
            uint64_t now = current_time();
            int counter = 5;

            for (auto itr = index.begin(); itr != index.end(); itr = index.begin()) {
                if (itr->expire > now) {
                    print(" itr->expire(", std::to_string((unsigned long long)itr->expire), ") > now(", std::to_string((unsigned long long)now) ,")\n");
                    break;
                }
                if (counter-- == 0) {
                    print(" No more than ",std::to_string(counter)," unstakes per action call\n");
                    break;
                }
                print(" unstaking: ", std::to_string((unsigned long long)itr->id), " - ", itr->quantity.to_string(), " - ", std::to_string((unsigned long long)itr->expire) ,"\n");
                
                // erase the unstakes entry
                asset quantity = itr->quantity;
                table.erase(*itr);

                auto tkn = tokens_table.find(quantity.symbol.code().raw());
                eosio_assert(tkn != tokens_table.end(), "token not registered");

                // return fouds to the owner
                action(
                    permission_level{get_self(),"active"_n},
                    tkn->contract,
                    "transfer"_n,
                    std::make_tuple(get_self(), owner, quantity, string("unstaking ") + quantity.to_string())
                ).send();
            }
            
            print("vapaee::token::stake::action_unstakeback() ...\n");
        }

        void action_unstaketime (name owner, const symbol_code & sym_code, uint64_t min_time, uint64_t max_time, uint64_t auto_stake) {
            print("vapaee::token::stake::action_unstaketime()\n");
            print(" owner: ", owner.to_string(), "\n");
            print(" sym_code: ", sym_code.to_string(), "\n");
            print(" min_time: ", std::to_string((unsigned long long) min_time), "\n");
            print(" max_time: ", std::to_string((unsigned long long) max_time), "\n");
            print(" auto_stake: ", std::to_string((unsigned long long) auto_stake), "\n");

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
        
        void action_delete_all_for(name owner) {
            print("vapaee::token::stake::action_delete_all_for()\n");
            print(" owner: ", owner.to_string(), "\n");

            require_auth(get_self());
            /*
            stakes stakes_table( get_self(), owner.value );
            for (auto ptr = stakes_table.begin(); ptr != stakes_table.end(); ptr = stakes_table.begin()) {
                print(" stakes_table.erase(*itr); ", std::to_string((unsigned long long)ptr->id), "\n");
                stakes_table.erase(ptr);
            }

            unstakes unstakes_table( get_self(), owner.value );
            for (auto ptr = unstakes_table.begin(); ptr != unstakes_table.end(); ptr = unstakes_table.begin()) {
                print(" unstakes_table.erase(*itr); ", std::to_string((unsigned long long)ptr->id), "\n");
                unstakes_table.erase(ptr);
            }
            */
            print("vapaee::token::stake::action_delete_all_for() ...\n");
        }

        
    }; // class

}; // namespace

}; // namespace