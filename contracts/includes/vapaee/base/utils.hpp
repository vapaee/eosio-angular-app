#pragma once
#include <vapaee/base/base.hpp>

using namespace std;
using namespace eosio;

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <cmath>

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

        // trim from start
        static inline std::string &ltrim(std::string &s) {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) {return !std::isspace(c);} ));
            return s;
        }

        // trim from end
        static inline std::string &rtrim(std::string &s) {
            s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) {return !std::isspace(c);}  ).base(), s.end());
            return s;
        }

        // trim from both ends
        static inline std::string &trim(std::string &s) {
            return ltrim(rtrim(s));
        }

        static inline int64_t str_to_int64(const std::string &s) {
            return std::stoi(s);
        }
        
        static asset string_to_asset(const std::string& str) {
            // print("vapaee::utils::string_to_asset()\n");
            // print(" str: ", str.c_str(), "\n");
        
            string s = str;
            s = vapaee::utils::trim(s);

            // Find space in order to split amount and symbol
            auto space_pos = s.find(' ');
            eosio_assert(space_pos != string::npos, "Asset's amount and symbol should be separated with space");
            std::string substring = s.substr(space_pos + 1);
            std::string symbol_str = vapaee::utils::trim(substring);
            std::string amount_str = s.substr(0, space_pos);

            // Ensure that if decimal point is used (.), decimal fraction is specified
            auto dot_pos = amount_str.find('.');
            if (dot_pos != string::npos) {
                eosio_assert(dot_pos != amount_str.size() - 1, "Missing decimal fraction after decimal point");
            }

            // Parse symbol
            string precision_digit_str;
            uint8_t precision_digit = 0;
            if (dot_pos != string::npos) {
                precision_digit = amount_str.size() - dot_pos - 1;
            }

            // string symbol_part = precision_digit_str + ',' + symbol_str;
            symbol sym = symbol(symbol_code(symbol_str), precision_digit);

            // Parse amount
            int64_t int_part, fract_part, amount;
            if (dot_pos != string::npos) {
                // print(" amount_str.substr(0, dot_pos): ", amount_str.substr(0, dot_pos).c_str(), "\n");
                // print(" amount_str.substr(dot_pos + 1): ", amount_str.substr(dot_pos + 1).c_str(), "\n");
                int_part = vapaee::utils::str_to_int64(amount_str.substr(0, dot_pos));
                fract_part = vapaee::utils::str_to_int64(amount_str.substr(dot_pos + 1));
                if (amount_str[0] == '-') {
                    fract_part *= -1;
                }
                // print(" int_part: ", std::to_string((long long) int_part), "\n");
                // print(" fract_part: ", std::to_string((long long) fract_part), "\n");
            } else {
                int_part = vapaee::utils::str_to_int64(amount_str);
            }

            int_part = int_part * pow(10, sym.precision());
            
            amount = int_part + fract_part;
            // print(" amount: ", std::to_string((long long) amount), "\n");
            // print("vapaee::utils::string_to_asset() ....\n");
            return asset(amount, sym);
        }        
    }; // namespace utils
}; // namespace vaapee
