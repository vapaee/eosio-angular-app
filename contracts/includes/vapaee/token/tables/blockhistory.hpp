#include <vapaee/token/tables/_aux.hpp>

        // TABLE blockhistory -----------
        // scope: xxx.tlos
        TABLE blockhistory_table {
            uint64_t hour;
            asset price;    // current price for this hour (and last)
            asset entrance; // first price for this hour
            asset max;      // max price for this hour
            asset min;      // min price for this hour            
            asset volume;
            time_point_sec date;
            uint64_t primary_key() const { return hour; }
        };
        typedef eosio::multi_index< "blockhistory"_n, blockhistory_table > blockhistory;
        // ------------------------------------