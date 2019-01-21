/*
#pragma once
#include <eosiolib/eosio.hpp>

using namespace std;
using namespace eosio;

namespace vapaee {

    // Structures --------------------------

    TABLE author_slug {
        uint64_t        id;
        name         owner;
        name      contract;
        slug          nick;
        string       title;
        uint64_t primary_key() const { return id;  }
        uint64_t by_owner_key() const { return owner.value;  }
        uint64_t by_contract_key() const { return contract.value;  }
        uint128_t secondary_key() const { return nick.to128bits(); }
        std::string to_string() const {
            return std::to_string((int) id) + " - " + owner.to_string() + " - " + nick.to_string()+ " - " + title + " - " + contract.to_string() ;
        };
    };
    
    typedef eosio::multi_index<"authors"_n, author_slug,
        indexed_by<"owner"_n, const_mem_fun<author_slug, uint64_t, &author_slug::by_owner_key>>,
        indexed_by<"contract"_n, const_mem_fun<author_slug, uint64_t, &author_slug::by_contract_key>>,
        indexed_by<"nick"_n, const_mem_fun<author_slug, uint128_t, &author_slug::secondary_key>>
    > author_slugs;

}
*/