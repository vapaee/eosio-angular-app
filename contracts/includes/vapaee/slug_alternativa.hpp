#pragma once
#include <iostream>
#include <string>
using namespace std;

#define longint unsigned long
#define maxlong 25

namespace vapaee {

    struct slug {
        longint top;
        longint low;
        
        slug():top(0),low(0) {}

        slug(longint n):top(0), low(n) {}

        slug(int n):top(0), low(n) {}

        slug(std::string str):top(0), low(0) {
            init(str);
        }

        slug(const char* str):top(0), low(0) {
            init(std::string(str));
        }

        void init(std::string str) {
            int n = std::min( (int) str.length(), maxlong );
            int i = 0;
            for(; i < n && i < sizeof(longint); ++i ) {
                top <<= 5;
                top |= char_to_value( str[i] );
            }
            for(; i < n ; ++i ) {
                low <<= 5;
                low |= char_to_value( str[i] );
            }
        }

        /**
         *  Converts a (eosio::name style) Base32 symbol into its corresponding value
         *
         *  @brief Converts a (eosio::name style) Base32 symbol into its corresponding value
         *  @param c - Character to be converted
         *  @return constexpr char - Converted value
         */
        static char char_to_value( char c ) {
            if( c == '.')
                return 0;
            else if( c >= '1' && c <= '5' )
                return (c - '1') + 1;
            else if( c >= 'a' && c <= 'z' )
                return (c - 'a') + 6;
            else
                cout << "character '" << c << "' is not in allowed character set for slug_symbol \n";
            // eosio_assert( false, (string("") + "character '" + c + "' is not in allowed character set for slug_symbol").c_str() );

            return 0; // control flow will never reach here; just added to suppress warning
        }           

        static char aux_int_to_hexa(int n) {
            if (n >= 0 && n <= 9) return '0' + n;
            if (n >= 10 && n <= 15) return 'A' + (n-10) ;
            cout << "ERROR converting " << n << " to hexa\n";
            return 'x';
        }

        static char aux_high_part(char c) {
            int v = c;
            v >>= 4;
            v = v & 0x0F;
            // cout << "high_part(" << (int)c << ") int: " << v << " int_to_hexa(v): " << int_to_hexa(v) << " \n";
            return aux_int_to_hexa(v);
        }

        static char aux_low_part(char c) {
            int v = c & 0x0F;
            return aux_int_to_hexa(v);
        }

        char* write_hexa_string( char* begin, char* end ) const {
            const char* ptr; char c;
            // constexpr uint64_t mask = 0x1Full;

            if( (begin + 64) < begin || (begin + 64) > end ) return begin;

            // auto v = value;
            ptr = (const char*) &top;
            for( int i = 0; i < sizeof(longint); i++) {
                c = *ptr;
                *begin = aux_high_part(c);
                ++begin;
                *begin = aux_low_part(c);
                ++begin;
                // ----
                ++ptr;
            }
            ptr = (const char*) &low;
            for( int i = 0; i < sizeof(longint); i++) {
                c = *ptr;
                *begin = aux_high_part(c);
                ++begin;
                *begin = aux_low_part(c);
                ++begin;
                // ----
                ++ptr;
            }

            return begin;
        }        

        std::string to_hexa() const {
            char buffer[64];
            char* end = write_hexa_string( buffer, buffer + sizeof(buffer) );
            *end = '\0';
            return std::string(buffer);
        }

        char* write_string( char* begin, char* end ) const {
            const char* ptr; char c;
            // constexpr uint64_t mask = 0x1Full;

            if( (begin + maxlong) < begin || (begin + maxlong) > end ) return begin;

            return begin;
        }        

        std::string to_string() const {
            char buffer[maxlong];
            char* end = write_string( buffer, buffer + sizeof(buffer) );
            *end = '\0';
            return std::string(buffer);
        }


        /**
         * Equivalency operator. Returns true if a == b (are the same)
         *
         * @brief Equivalency operator
         * @return boolean - true if both provided slug are the same
         */
        friend bool operator == ( const slug& a, const slug& b ) {
            return a.top == b.top && a.low == b.low;
        }

        /**
         * Inverted equivalency operator. Returns true if a != b (are different)
         *
         * @brief Inverted equivalency operator
         * @return boolean - true if both provided slug are not the same
         */
        friend bool operator != ( const slug& a, const slug& b ) {
            return a.top != b.top || a.low != b.low;
        }

        /**
         * Less than operator. Returns true if a < b.
         * @brief Less than operator
         * @return boolean - true if slug `a` is less than `b`
         */
        friend bool operator < ( const slug& a, const slug& b ) {
            if (a.top != b.top) a.top < b.top;
            return a.low < b.low;
        }        
    };

}; /// namespace eosio
