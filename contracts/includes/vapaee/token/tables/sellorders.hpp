#include <vapaee/token/tables/_aux.hpp>

        // TABLE sellorders (registered currency) -----------
        // scope: symbol_code_A.symbol_code_P
        TABLE sell_order_table {
            uint64_t id;
            name owner;
            asset price; // symbol_code_P - how much P per A unit
            asset amount; // symbol_code_P - how much P you want
            asset deposit; // symbol_code_A - how much A you are selling
            uint64_t primary_key() const { return id; }
            uint64_t by_price_key() const { return price.amount; } // return asset::max_amount
            // uint64_t by_inverse_key() const { ((double)price.amount / (double)pow(10.0, price.symbol.precision())) / 1.0; }
            uint64_t by_owner_key() const { return owner.value; }
            uint64_t by_amount_key() const { return amount.amount; }
        };

        typedef eosio::multi_index< "sellorders"_n, sell_order_table,
            indexed_by<"price"_n, const_mem_fun<sell_order_table, uint64_t, &sell_order_table::by_price_key>>
            // ,
            // indexed_by<"inverse"_n, const_mem_fun<sell_order_table, uint64_t, &sell_order_table::by_inverse_key>>
            // indexed_by<"owner"_n, const_mem_fun<sell_order_table, uint64_t, &sell_order_table::by_owner_key>>,
            // indexed_by<"amount"_n, const_mem_fun<sell_order_table, uint64_t, &sell_order_table::by_amount_key>>
        > sellorders;
        // ------------------------------------
        