#include <vapaee/token/tables/_aux.hpp>

        // TABLE tablesummary -----------
        // scope: contract
        TABLE tablesummary_table {
            name table;
            symbol_code sell;
            symbol_code pay;
            asset total;
            vector<asset> fees;
            int orders;
            uint64_t primary_key() const { return table.value; }
        };
        typedef eosio::multi_index< "tablesummary"_n, tablesummary_table > tablesummary;
        // ------------------------------------