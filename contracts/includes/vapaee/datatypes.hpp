#pragma once
#include <eosiolib/eosio.hpp>
#include <vapaee/vapaee_aux_functions.hpp>

using namespace std;
using namespace eosio;

#define FULL64BITS 0xFFFFFFFFFFFFFFFF

namespace vapaee {

    struct slotinfo {
        uint64_t position;
        uint64_t container;
        slotinfo():position(0), container(0) {}
        slotinfo(const slotinfo &c):position(c.position), container(c.container) {}
        uint128_t to128bits() const { return vapaee::combine(container, position); }
        void setAux() {
            position = FULL64BITS;
            container = FULL64BITS;
        }
        bool isAux() const {
            return position == FULL64BITS && container == FULL64BITS;
        }        
        EOSLIB_SERIALIZE(slotinfo, (position)(container))
    };    

    struct requireinfo {
        uint8_t points;
        uint8_t mastery;
        EOSLIB_SERIALIZE(requireinfo, (points)(mastery))
    };

    struct levelinfo {
        uint8_t value;
        requireinfo require;
        EOSLIB_SERIALIZE(levelinfo, (value)(require))
    };

    struct iconinfo {
        string small;
        string big;
        EOSLIB_SERIALIZE(iconinfo, (small)(big))
    };

    struct mastery_property {
        name title;
        name desc;
        slug property;
        iconinfo icon;        
        std::vector<levelinfo> levels;

        EOSLIB_SERIALIZE(mastery_property, (title)(desc)(property)(icon)(levels))
    };

};