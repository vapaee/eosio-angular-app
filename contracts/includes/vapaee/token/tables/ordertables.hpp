#include <vapaee/token/tables/_aux.hpp>

        // TABLE ordertables -----------
        // scope: contract
        TABLE ordertables_table {
            name label;
            asset price;
            asset volumen;
            string concept;
            uint64_t primary_key() const { return table.value; }
        };
        typedef eosio::multi_index< "ordertables"_n, ordertables_table > ordertables;
        // ------------------------------------