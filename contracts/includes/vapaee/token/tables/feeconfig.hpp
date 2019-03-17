#include <vapaee/token/tables/_aux.hpp>

        // TABLE feeconfig -----------
        // scope: contract
        TABLE feeconfig_table {
            asset fee;
            uint64_t primary_key() const { return fee.symbol.code().raw(); }
        };
        typedef eosio::multi_index< "feeconfig"_n, feeconfig_table> feeconfig;
        // ------------------------------------
        