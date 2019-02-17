#include <vapaee/token/tables/_aux.hpp>

        // TABLE buyorders (registered currency) -----------
        // scope: symbol_code_A.symbol_code_P
        TABLE buy_order_table {
            uint64_t id;
            name owner;
            asset price; // symbol_code_P
            asset amount; // symbol_code_A
            asset deposit; // symbol_code_P
            uint64_t primary_key() const { return id; }
            uint64_t by_price_key() const { return asset::max_amount - price.amount; }
            uint64_t by_owner_key() const { return owner.value; }
            uint64_t by_amount_key() const { return amount.amount; }
        };

        typedef eosio::multi_index< "buyorders"_n, buy_order_table,
            indexed_by<"price"_n, const_mem_fun<buy_order_table, uint64_t, &buy_order_table::by_price_key>>
            // ,
            // indexed_by<"owner"_n, const_mem_fun<buy_order_table, uint64_t, &buy_order_table::by_owner_key>>,
            // indexed_by<"amount"_n, const_mem_fun<buy_order_table, uint64_t, &buy_order_table::by_amount_key>>
        > buyorders;
        // ------------------------------------
        