#pragma once
#include <eosiolib/eosio.hpp>

using namespace std;
using namespace eosio;

struct my_datatype {
    uint128_t num;

    constexpr explicit my_datatype(): num(0) {}
    constexpr explicit my_datatype(const char* s): num(0) {}
    constexpr explicit my_datatype(const std::string_view &s): num(0) {}
    
    EOSLIB_SERIALIZE(my_datatype, (num))
};

