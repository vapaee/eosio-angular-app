#include <vapaee/token/tables/_aux.hpp>

        // TABLE ordertables -----------
        // scope: owner
        TABLE ordertables_table {
            name table;
            symbol_code sell;
            symbol_code pay;
            asset total;
            vector<asset> fees;
            int orders;
            uint64_t primary_key() const { return table.value; }
        };
        typedef eosio::multi_index< "ordertables"_n, ordertables_table > ordertables;
        // ------------------------------------
        