#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <vapaee/slug_asset.hpp>

using namespace eosio;
namespace vapaee {

CONTRACT vapaeeauthor : public eosio::contract {
    public:

        using contract::contract;

        ACTION registernick(name owner, slug nick) {

        }

        ACTION transfernick(name owner, name newowner, slug nick) {
            
        }

        ACTION transferid(name owner, name newowner, uint64_t id) {
            
        }

    private:
        TABLE author_slug { // scope: self
            uint64_t        id;
            name         owner;
            slug          nick;
            uint64_t primary_key() const { return id;  }
            uint64_t by_owner_key() const { return owner.value;  }
            uint128_t secondary_key() const { return nick.to128bits(); }
        };
        
        typedef eosio::multi_index<"authors"_n, author_slug,
            indexed_by<"owner"_n, const_mem_fun<author_slug, uint64_t, &author_slug::by_owner_key>>,
            indexed_by<"second"_n, const_mem_fun<author_slug, uint128_t, &author_slug::secondary_key>>
        > author_slugs;

};

}; // namespace