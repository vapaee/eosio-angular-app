#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;

CONTRACT bounds : public eosio::contract {
  
    public:
        using contract::contract;

        ACTION load() {
            numbers table(get_self(), get_self().value);
            table.emplace(get_self(), [&]( auto& row ) { row.id = 0; row.num = current_time() + 60; });
            table.emplace(get_self(), [&]( auto& row ) { row.id = 1; row.num = current_time() + 20; });
            table.emplace(get_self(), [&]( auto& row ) { row.id = 2; row.num = current_time() + 40; });
            table.emplace(get_self(), [&]( auto& row ) { row.id = 3; row.num = current_time() + 30; });
            table.emplace(get_self(), [&]( auto& row ) { row.id = 4; row.num = current_time() + 50; });
            table.emplace(get_self(), [&]( auto& row ) { row.id = 5; row.num = current_time() + 70; });
        }

        ACTION show() {
            numbers table(get_self(), get_self().value);
            auto index = table.template get_index<"bynum"_n>();

            print("lower_bound(1550028096500040) ------------\n");
            for (auto ptr = index.lower_bound(1550028096500040); ptr != index.end(); ptr++) {
                print(" ptr: ", std::to_string((unsigned long long)ptr->id), " - ", std::to_string((unsigned long long)ptr->num) ,"\n");
            }
            
            print("upper_bound(1550028096500040) ------------\n");
            for (auto ptr = index.upper_bound(1550028096500040); ptr != index.begin(); ptr--) {
                print(" ptr: ", std::to_string((unsigned long long)ptr->id), " - ", std::to_string((unsigned long long)ptr->num) ,"\n");
            }
        }

        ACTION clear() {
            numbers table(get_self(), get_self().value);
            table.erase(table.begin());
            table.erase(table.begin());
            table.erase(table.begin());
            table.erase(table.begin());
            table.erase(table.begin());
            table.erase(table.begin());
            
        }

    private:
        TABLE my_table {
            uint64_t id;
            uint64_t num;
            uint64_t primary_key() const { return id; }
            uint64_t by_num_key() const { return num; }
        };

        typedef eosio::multi_index< "numbers"_n, my_table,
            indexed_by<"bynum"_n, const_mem_fun<my_table, uint64_t, &my_table::by_num_key>>
        > numbers;
        
};

EOSIO_DISPATCH( bounds, (load)(show)(clear));
