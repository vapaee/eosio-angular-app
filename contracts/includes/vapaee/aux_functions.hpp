#pragma once
#include <eosiolib/eosio.hpp>

using namespace std;
using namespace eosio;

namespace vapaee {

    static uint128_t combine( name nick, uint64_t author ) {
        return (uint128_t{nick.value} << 64) | uint128_t{author};
    }

    /*
    char int_to_hexa(int n) const {
        if (n >= 0 && n <= 9) return '0' + n;
        if (n >= 10 && n <= 15) return 'A' + (n-10);
        eosio_assert( false, (string("") + "ERROR converting '" + n + "' to hexa. ").c_str() );
        return 'x';
    }

    std::string int_to_nibble(char n) const {
        char ptr[10];
        ptr[4] = ' ';
        ptr[9] = '\0';
        for (int i=0, j=0; i<8; i++) {
            j=i;
            if (i>3) j=i+1;
            ptr[j] = ( n & (0x01 << (7-i)) ) ? '1' : '0';
        }
        return std::string(ptr);
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

        if( (begin + nibble) < begin || (begin + nibble) > end ) return begin;

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

    std::string to_hexa() const {
        char buffer[nibble];
        char* end = write_hexa_string( buffer, buffer + sizeof(buffer) );
        *end = '\0';
        return std::string(buffer);
    }
    */
}