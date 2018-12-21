#pragma once
#include <eosiolib/eosio.hpp>
#include <vapaee/vapaee_aux_functions.hpp>

using namespace std;
using namespace eosio;

namespace vapaee {

    // Structures --------------------------

    TABLE author_slug {
        uint64_t        id;
        name         owner;
        slug          nick;
        uint64_t primary_key() const { return id;  }
        uint64_t by_owner_key() const { return owner.value;  }
        uint128_t secondary_key() const { return nick.to128bits(); }
        std::string to_string() const {
            return std::string(" owner: ") + owner.to_string() + " - " + nick.to_string();
        };
    };
    
    typedef eosio::multi_index<"authors"_n, author_slug,
        indexed_by<"owner"_n, const_mem_fun<author_slug, uint64_t, &author_slug::by_owner_key>>,
        indexed_by<"nick"_n, const_mem_fun<author_slug, uint128_t, &author_slug::secondary_key>>
    > author_slugs; 

    // API --------------------------------

    static uint128_t combine( uint64_t key1, uint64_t key2 ) {
        return (uint128_t{key1} << 64) | uint128_t{key2};
    }

    static uint128_t combine(uint64_t key1, name key2 ) {
        // return (uint128_t{key1.value} << 64) | uint128_t{key2};
        return vapaee::combine(key1, key2.value);
    }

    static uint128_t combine( name key1, name key2 ) {
        return vapaee::combine(key1.value, key2.value);
    }

    static name get_author_owner(uint64_t author_id) {
        author_slugs authors("vapaeeauthor"_n, ("vapaeeauthor"_n).value);
        auto itr = authors.find(author_id);
        eosio_assert(itr != authors.end(), "Author id does NOT exist");
        return itr->owner;
    }

    static slug get_author_nick(uint64_t author_id) {
        author_slugs authors("vapaeeauthor"_n, ("vapaeeauthor"_n).value);
        auto itr = authors.find(author_id);
        eosio_assert(itr != authors.end(), "Author id does NOT exist");
        return itr->nick;
    }

    static void get_authors_for_owner(name owner, std::vector<uint64_t> &authors) {
        // owner index
        auto index_owner = this->authors.template get_index<"owner"_n>();
        for (auto itr = index_owner.lower_bound(owner); itr != index_owner.end() && itr->owner == owner; itr++;) {
            authors.push(itr->id);
        }
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