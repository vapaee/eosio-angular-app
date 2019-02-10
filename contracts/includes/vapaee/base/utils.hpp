#pragma once
#include <vapaee/base/base.hpp>

using namespace std;
using namespace eosio;

namespace vapaee {
    namespace utils {
        static uint128_t combine( uint64_t key1, uint64_t key2 ) {
            return (uint128_t{key1} << 64) | uint128_t{key2};
        }

        static uint128_t combine( uint64_t key1, name key2 ) {
            // return (uint128_t{key1.value} << 64) | uint128_t{key2};
            return vapaee::utils::combine(key1, key2.value);
        }

        static uint128_t combine( name key1, name key2 ) {
            return vapaee::utils::combine(key1.value, key2.value);
        }
    }; // namespace utils
}; // namespace vaapee
