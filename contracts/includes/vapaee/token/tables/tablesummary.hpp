#include <vapaee/token/tables/_aux.hpp>

        // TABLE tablesummary -----------
        // scope: xxx.tlos
        TABLE tablesummary_table {
            name label;
            asset price;
            asset volumen;
            string concept;
            uint64_t primary_key() const { return label.value; }
        };
        typedef eosio::multi_index< "tablesummary"_n, tablesummary_table > tablesummary;
        // ------------------------------------