#include <vapaee/token/tables/_aux.hpp>

        // TABLE feeconfig -----------
        // scope: contract
        TABLE feeconfig_table {
            uint64_t priority;
            asset fee;
            uint64_t primary_key() const { return priority; }
            uint64_t by_symbol_key() const { return fee.symbol.code().raw(); }
        };
        typedef eosio::multi_index< "feeconfig"_n, feeconfig_table,
            indexed_by<"symbol"_n, const_mem_fun<feeconfig_table, uint64_t, &feeconfig_table::by_symbol_key>>
        > feeconfig;
        // ------------------------------------
        