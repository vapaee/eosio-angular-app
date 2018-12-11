#pragma once
#include <iostream>
#include <string>
using namespace std;

namespace vapaee {

    struct slug {
        char value[ 32 ]; // 256 bits
        slug() {
            init(0);
        }

        slug(unsigned long i) {
            init(i);
        }

        slug(const char* s) {
            init(0);
        }

        void init(unsigned long i) {
            char* ptr = value;
            unsigned long* d = (unsigned long*) ptr;
            *d = 0;
            d = (unsigned long*) (ptr + 8);
            *d = 0;
            d = (unsigned long*) (ptr + 16);
            *d = 0;
            d = (unsigned long*) (ptr + 24);
            *d = i;
        }

        char int_to_hexa(int n) const {
            if (n >= 0 && n <= 9) return '0' + n;
            if (n >= 10 && n <= 15) return 'A' + (n-10) ;
            cout << "ERROR converting " << n << " to hexa\n";
            return 'x';
        }

        char high_part(char c) const {
            int v = c;
            v >>= 4;
            v = v & 0x0F;
            // cout << "high_part(" << (int)c << ") int: " << v << " int_to_hexa(v): " << int_to_hexa(v) << " \n";
            return int_to_hexa(v);
        }

        char low_part(char c) const {
            int v = c & 0x0F;
            return int_to_hexa(v);
        }

        char* write_hexa_string( char* begin, char* end ) const {
            // constexpr uint64_t mask = 0x1Full;

            if( (begin + 64) < begin || (begin + 64) > end ) return begin;

            // auto v = value;
            const char* ptr = value;
            char c;
            for( int i = 0; i < 32; i++) {
                // if( v == 0 ) return begin;
                c = *ptr;
                *begin = high_part(c);
                ++begin;
                *begin = low_part(c);
                ++begin;
                // ----
                ++ptr;
            }

            return begin;
        }        

        std::string to_string() const {
            char buffer[64];
            char* end = write_hexa_string( buffer, buffer + sizeof(buffer) );
            *end = '\0';
            return std::string(buffer);
        }
    };

}; /// namespace eosio
