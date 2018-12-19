#pragma once
#include <eosiolib/eosio.hpp>

using namespace std;
using namespace eosio;

namespace vapaee {

    struct requireinfo {
        uint8_t points;
        uint8_t mastery;
        EOSLIB_SERIALIZE(requireinfo, (points)(mastery))
    }

    struct levelinfo {
        uint8_t value;
        requireinfo require;
        EOSLIB_SERIALIZE(levelinfo, (value)(require))
    }

    struct iconinfo {
        string small;
        string big;
        EOSLIB_SERIALIZE(iconinfo, (small)(big))
    }

    struct mastery_property {
        string name;
        string desc;
        slug property;
        iconinfo icon;        
        std::vector<levelinfo> levels;

        EOSLIB_SERIALIZE(mastery_property, (name)(desc)(property)(icon)(levels))
    };

};