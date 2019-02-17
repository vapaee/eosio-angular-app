// #ifndef TABLE
#pragma once
#include <eosiolib/eosio.hpp>
#include <eosiolib/symbol.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/transaction.hpp>


// defining namespaces 
using namespace eosio;

namespace vapaee {
    namespace bgbox {
        static name contract = "boardgamebox"_n;
    };
    namespace token {
        static name contract = "vapaeetokens"_n;
    };
    namespace cnt {
        static name contract = "cardsntokens"_n;
    };
    namespace utils {}
};

using namespace vapaee;
using namespace bgbox;
using namespace cnt;
using namespace utils;

#include <vapaee/base/slug.hpp>
#include <vapaee/base/slug_asset.hpp>
#include <vapaee/base/slug_symbol.hpp>
#include <vapaee/base/utils.hpp>

// #endif

