#include <vapaee/token/tables/_aux.hpp>

        // TABLE history -----------
        // scope: xxx.zzz
        TABLE history_table {
            uint64_t id;
            uint64_t date;
            name buyer;
            name seller;
            asset amount;
            asset price;
            asset buyfee;
            asset sellfee;
            uint64_t primary_key() const { return id; }
            uint64_t by_date_key() const { return date; }
            uint64_t by_buyer_key() const { return buyer.value; }
            uint64_t by_seller_key() const { return seller.value; }
        };
        typedef eosio::multi_index< "history"_n, history_table,
            indexed_by<"date"_n, const_mem_fun<history_table, uint64_t, &history_table::by_date_key>>,
            indexed_by<"buyer"_n, const_mem_fun<history_table, uint64_t, &history_table::by_buyer_key>>,
            indexed_by<"seller"_n, const_mem_fun<history_table, uint64_t, &history_table::by_seller_key>>
        > history;
        // ------------------------------------
        