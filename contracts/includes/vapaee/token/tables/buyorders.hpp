#include <vapaee/token/tables/_aux.hpp>

        // TABLE buyorders (registered currency) -----------
        // scope: symbol_code_A.symbol_code_P
        TABLE order_table {
            uint64_t id;
            name owner;
            asset price; // symbol_code_P
            asset amount; // symbol_code_A
            uint64_t primary_key() const { return symbol.raw(); }
            uint64_t by_price_key() const { return price.amount; }            
            uint64_t by_amount_key() const { return amount.amount; }
        };

        typedef eosio::multi_index< "buyorders"_n, order_table,
            indexed_by<"price"_n, const_mem_fun<order_table, uint64_t, &order_table::by_price_key>>
            indexed_by<"amount"_n, const_mem_fun<order_table, uint64_t, &order_table::by_amount_key>>
        > buyorders;
        // ------------------------------------
        