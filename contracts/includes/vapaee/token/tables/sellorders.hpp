#include "buyorders.hpp"

        typedef eosio::multi_index< "sellorders"_n, order_table,
            indexed_by<"price"_n, const_mem_fun<order_table, uint64_t, &order_table::by_price_key>>
            indexed_by<"amount"_n, const_mem_fun<order_table, uint64_t, &order_table::by_amount_key>>
        > sellorders;
        // ------------------------------------
        