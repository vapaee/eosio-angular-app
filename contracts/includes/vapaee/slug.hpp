#pragma once
#include <iostream>
#include <string>
using namespace std;

#define longint unsigned long
#define maxlong 50
#define bytes 32
#define nibble 62 
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
            char v, c;
            int n = std::min( (int) str.length(), maxlong );
            int i = 0;
            int shift;
            int ibit, jbit, offset;

            for(; i < n; ++i ) {
                ibit = i*5;
                int j = (int)(ibit/8);
                jbit = j*8;
                offset = ibit - jbit;
                shift = 3 - offset;
                /*
                cout
                    << " i: " << i << " j: " << j
                    << " ibit: " << ibit
                    << " jbit: " << jbit
                    << " offset: " << offset
                    << " shift: " << shift
                    << "\n";
                */
                    /*
                    i: 0 j: 0 ibit: 0 jbit: 0 offset: 0 shift: 3
                    i: 1 j: 0 ibit: 5 jbit: 0 offset: 5 shift: -2
                    i: 2 j: 1 ibit: 10 jbit: 8 offset: 2 shift: 1
                    i: 3 j: 1 ibit: 15 jbit: 8 offset: 7 shift: -4
                    i: 4 j: 2 ibit: 20 jbit: 16 offset: 4 shift: -1
                    i: 5 j: 3 ibit: 25 jbit: 24 offset: 1 shift: 2
                    i: 6 j: 3 ibit: 30 jbit: 24 offset: 6 shift: -3
                    i: 7 j: 4 ibit: 35 jbit: 32 offset: 3 shift: 0

                    // Yxxx x000 - 0000 0000 - 0000 0000 - 0000 0000 - 0000 0000
                    // 0000 0Yxx - xx00 0000 - 0000 0000 - 0000 0000 - 0000 0000
                    // 0000 0000 - 00Yx xxx0 - 0000 0000 - 0000 0000 - 0000 0000
                    // 0000 0000 - 0000 000Y - xxxx 0000 - 0000 0000 - 0000 0000
                    // 0000 0000 - 0000 0000 - 0000 Yxxx - x000 0000 - 0000 0000
                    // 0000 0000 - 0000 0000 - 0000 0000 - 0Yxx xx00 - 0000 0000
                    // 0000 0000 - 0000 0000 - 0000 0000 - 0000 00Yx - xxx0 0000
                    // 0000 0000 - 0000 0000 - 0000 0000 - 0000 0000 - 000Y xxxx

                    */
                v = char_to_value(str[i]);
                // cout << str[i] << " i:" << i << " j:" << j << " - " << to_hexa() << "\n";                
                if (shift >= 0) {
                    c = v << shift;
                    // cout << "v:          " << int_to_nibble(v) << "\n";
                    // cout << "<< " << shift << "        " << int_to_nibble(c) << "\n";
                    // cout << "ptr[j]      " << int_to_nibble(ptr[j]) << "\n";
                    ptr[j] |= c;
                    // cout << "ptr[j] | c  " << int_to_nibble(ptr[j]) << "\n";
                } else {
                    c = v >> (-1 * shift);
                    // cout << "v:          " << int_to_nibble(v) << "\n";
                    // cout << ">> " << (-1 * shift) << "        " << int_to_nibble(c) << "\n";
                    // cout << "ptr[j]      " << int_to_nibble(ptr[j]) << "\n";
                    
                    ptr[j] |= c;
                    // cout << "ptr[j] | c  " << int_to_nibble(ptr[j]) << "\n";
                    if (j<bytes) {
                        c = v << 8+shift;
                        // cout << "c:          " << int_to_nibble(c) << "\n";
                        // cout << "ptr[j+1]    " << int_to_nibble(ptr[j+1]) << "\n";
                        ptr[j+1] |= c;
                        // cout << "ptr[j+1]|c  " << int_to_nibble(ptr[j+1]) << "\n";
                    } else {
                        cout << "AAAAA";
                    }
                    // cout << "0x" << high_part(v >> (-1 * shift)) << low_part(v >> (-1 * shift)) << "\n";
                }
            }
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

        static char value_to_char( char v ) {
            if (v == 0) return '.';
            if (v < 6) return v + '0';
            if (v < 32) return v + 'a' - 6;
            cout << "value '" << v << "' is out of range for slug_symbol \n";
            return '?';
        }       

        char int_to_hexa(int n) const {
            if (n >= 0 && n <= 9) return '0' + n;
            if (n >= 10 && n <= 15) return 'A' + (n-10) ;
            cout << "ERROR converting " << n << " to hexa\n";
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
     

        char* write_string( char* begin, char* end ) const {
            const char* ptr = value;
            char v, c;
            int i = 0, s = 0;
            int shift;
            int ibit, jbit, offset;
            if( (begin + maxlong) < begin || (begin + maxlong) > end ) return begin;

            for(i=maxlong-1; i >= 0; i--) {
                ibit = i*5;
                int j = (int)(ibit/8);
                jbit = j*8;
                offset = ibit - jbit;
                shift = 3 - offset;
                /*
                cout
                    << " i: " << i << " j: " << j
                    << " ibit: " << ibit
                    << " jbit: " << jbit
                    << " offset: " << offset
                    << " shift: " << shift
                    << "\n";
                    */
                    /*
                    i: 0 j: 0 ibit: 0 jbit: 0 offset: 0 shift: 3
                    i: 1 j: 0 ibit: 5 jbit: 0 offset: 5 shift: -2
                    i: 2 j: 1 ibit: 10 jbit: 8 offset: 2 shift: 1
                    i: 3 j: 1 ibit: 15 jbit: 8 offset: 7 shift: -4
                    i: 4 j: 2 ibit: 20 jbit: 16 offset: 4 shift: -1
                    i: 5 j: 3 ibit: 25 jbit: 24 offset: 1 shift: 2
                    i: 6 j: 3 ibit: 30 jbit: 24 offset: 6 shift: -3
                    i: 7 j: 4 ibit: 35 jbit: 32 offset: 3 shift: 0

                    // Yxxx x000 - 0000 0000 - 0000 0000 - 0000 0000 - 0000 0000 | 3
                    // 0000 0Yxx - xx00 0000 - 0000 0000 - 0000 0000 - 0000 0000 | -2 
                    // 0000 0000 - 00Yx xxx0 - 0000 0000 - 0000 0000 - 0000 0000 | 1
                    // 0000 0000 - 0000 000Y - xxxx 0000 - 0000 0000 - 0000 0000 | -4
                    // 0000 0000 - 0000 0000 - 0000 Yxxx - x000 0000 - 0000 0000 | -1
                    // 0000 0000 - 0000 0000 - 0000 0000 - 0Yxx xx00 - 0000 0000 | 2
                    // 0000 0000 - 0000 0000 - 0000 0000 - 0000 00Yx - xxx0 0000 | -3
                    // 0000 0000 - 0000 0000 - 0000 0000 - 0000 0000 - 000Y xxxx | 0
                    */
                // cout << int_to_nibble(ptr[j]) << " i:" << i << " j:" << j << " shift:" << shift << "\n";                   
                if (shift >= 0) {
                    v = (31 << shift);
                    // cout << "mask1:      " << int_to_nibble(v) << "\n";
                    // cout << "ptr[j]:     " << int_to_nibble(ptr[j]) << "\n";
                    c = ptr[j] & v;
                    // cout << "ptr[j] & v: " << int_to_nibble(c) << "\n";
                    if (shift > 0) {
                        c >>= 1;
                        c &= 0x7F; // masking 0111 1111 ------> clear bit 0 
                        c >>= (shift-1); // now we can shift safely
                    }
                    // cout << ">>  "<< shift <<"       " << int_to_nibble(c) << " ------> " << value_to_char(c) << "\n";
                } else {
                    v = 31 >> (-1 * shift);
                    // cout << "mask1:      " << int_to_nibble(v) << "\n";
                    c = ptr[j] & v;
                    // cout << "ptr[j]:     " << int_to_nibble(ptr[j]) << "\n";
                    // cout << "ptr[j] & v: " << int_to_nibble(c) << "\n";
                    // 0000 0Yxx
                    c <<= -1 * shift;
                    // cout << "<< " << (-1*shift)<< "        " << int_to_nibble(c) << " <<< c \n";
                    // 000Y xx00
                    if (j<bytes) {
                        v = 31 << (8+shift);
                        // cout << "mask2:      " << int_to_nibble(v) << "\n";
                        // cout << "ptr[j+1]:   " << int_to_nibble(ptr[j+1]) << "\n";
                        v = ptr[j+1] & v;
                        // cout << "ptr[j+1]&v: " << int_to_nibble(v) << "\n";
                        // xx00 0000
                        v >>= 1;
                        v &= 0x7F; // masking 0111 1111 ------> clear bit 0 
                        v >>= (7+shift); // now we can shift safely
                        // cout << ">>  "<< (8+shift) <<"       " << int_to_nibble(v) << "\n";
                        c = c | v;
                        // cout << "c | v:      " << int_to_nibble(c) << " ------> " << value_to_char(c) << "\n";
                    } else {
                        cout << "BBBBBB";
                    }
                }
                /*
                cout
                    << " i: " << i << " j: " << j
                    << " ibit: " << ibit
                    << " jbit: " << jbit
                    << " offset: " << offset
                    << " shift: " << shift                
                    << "  v: " << int_to_nibble(v)
                    << "  ptr[j]: " << int_to_nibble(ptr[j])
                    << "  c: " << int_to_nibble(c)
                    << " " << value_to_char(c) << "\n";
                    */
                
                s += c;
                // cout << i << " ssssssssss: " << s << "\n";
                if (s > 0) {
                    begin[i] = value_to_char(c);
                } else {
                    begin[i] = '\0';
                }                
            }
            return begin+i;
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
            return
                *((longint*) (a.value + 0))  == *((longint*) (b.value + 0))  &&
                *((longint*) (a.value + 8))  == *((longint*) (b.value + 8))  &&
                *((longint*) (a.value + 16)) == *((longint*) (b.value + 16)) &&
                *((longint*) (a.value + 24)) == *((longint*) (b.value + 24));
        }

        /**
         * Inverted equivalency operator. Returns true if a != b (are different)
         *
         * @brief Inverted equivalency operator
         * @return boolean - true if both provided slug are not the same
         */
        friend bool operator != ( const slug& a, const slug& b ) {
            return
                *((longint*) (a.value + 0))  != *((longint*) (b.value + 0))  ||
                *((longint*) (a.value + 8))  != *((longint*) (b.value + 8))  ||
                *((longint*) (a.value + 16)) != *((longint*) (b.value + 16)) ||
                *((longint*) (a.value + 24)) != *((longint*) (b.value + 24));
        }

        /**
         * Less than operator. Returns true if a < b.
         * @brief Less than operator
         * @return boolean - true if slug `a` is less than `b`
         */
        friend bool operator < ( const slug& a, const slug& b ) {
            if ( *((longint*) (a.value +  0))  !=  *((longint*) (b.value +  0)) )
                return *((longint*) (a.value +  0)) < *((longint*) (b.value +  0));

            if ( *((longint*) (a.value +  8))  !=  *((longint*) (b.value +  8)) )
                return *((longint*) (a.value +  8)) < *((longint*) (b.value +  8));

            if ( *((longint*) (a.value + 16))  !=  *((longint*) (b.value + 16)) )
                return *((longint*) (a.value + 16)) < *((longint*) (b.value + 16));

            return *((longint*) (a.value + 24)) < *((longint*) (b.value + 24));
        }               

    };

};