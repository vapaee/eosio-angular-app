#pragma once
#include <iostream>
#include <string>
using namespace std;

#define longint unsigned long
#define maxlong 50
#define bytes 32
#define bits 256

namespace vapaee {

    struct slug {
        char value[ bytes ];
        slug() {
            init(0);
        }

        slug(unsigned long i) {
            init(i);
        }

        slug(const char* s) {
            init(0);
            init(s);
        }

        void init(longint i) {
            char* ptr = value;
            longint* d = (longint*) ptr;
            *d = 0;
            d = (longint*) (ptr + 8);
            *d = 0;
            d = (longint*) (ptr + 16);
            *d = 0;
            d = (longint*) (ptr + 24);
            *d = i;
        }

        void init(std::string str) {
            char* ptr = value;
            longint* d = (longint*) ptr;
            char v, c;
            int n = std::min( (int) str.length(), maxlong );
            int i = 0; 

            cout << "to_hexa():  " << to_hexa() << "\n"; 
            for(; i < n && i < sizeof(longint); ++i ) {
                c = str[i];
                v = char_to_value( c );
                *d <<= 5;
                cout << "  *d <<= 5; " << to_hexa() << " -- c: " << c << "\n"; 
                *d |= v;
                cout << "  *d |= v;  " << to_hexa() << " -- v: " << (int) v << "\n"; 
            }


            /*
            int n = std::min( (int) str.length(), maxlong );
            int i = 0;
            for(; i < n && i < sizeof(longint); ++i ) {
                top <<= 5;
                top |= char_to_value( str[i] );
            }
            for(; i < n ; ++i ) {
                low <<= 5;
                low |= char_to_value( str[i] );
            }*/
        }

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

        std::string to_hexa() const {
            char buffer[64];
            char* end = write_hexa_string( buffer, buffer + sizeof(buffer) );
            *end = '\0';
            return std::string(buffer);
        }
        

        char* write_string( char* begin, char* end ) const {
            // constexpr uint64_t mask = 0x1Full;

            if( (begin + 64) < begin || (begin + 64) > end ) return begin;

            return begin;
        }        

        std::string to_string() const {
            char buffer[64];
            char* end = write_string( buffer, buffer + sizeof(buffer) );
            *end = '\0';
            return std::string(buffer);
        }        

    };

};