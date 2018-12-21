#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <vapaee/slug_asset.hpp>
#include <vapaee/mastery_property.hpp>
#include <vapaee/vapaee_aux_functions.hpp>

#define inventary_default_space 8

using namespace eosio;
namespace vapaee {

CONTRACT localstrings : public eosio::contract {
    public:
        localstrings(name receiver, name code, datastream<const char*> ds):contract(receiver, code, ds) {}

        ACTION newstring(uint64_t registerer, name key, string text, name local) {
            // verificar que no exista ya el mismo key
            strings table(get_self(), get_self().value);
            auto itr = table.find(key.value);
            eosio_assert(itr == table.end(), "key already registered");

            // verify author and owner signature
            name owner = vapaee::get_author_owner(registerer);
            require_auth(owner);

            // insert entry
            table.emplace( owner, [&]( auto& s ) {
                s.registerer = registerer;
                s.key        = key;
            });

            action(
                permission_level{owner,"active"_n},
                get_self(),
                "newlocal"_n,
                std::make_tuple(registerer, text, local, "official"_n)
            ).send();
        };

        ACTION newlocal(uint64_t registerer, string text, name stringkey, name local, name version) {

            // verify author and owner signature
            name owner = vapaee::get_author_owner(registerer);
            require_auth(owner);

            strings str_table(get_self(), get_self().value);
            auto itr = str_table.find(stringkey.value);
            eosio_assert(itr != str_table.end(), (string("String with key '") + stringkey.to_string() + "' does not exist").c_str());

            // if  version official registerer must be the entry.registerer
            if (version == "official"_n) {
                eosio_assert(itr->registerer == registerer, "Only the string registerer can create official local for this string entry");
            }

            locals loc_table(get_self(), stringkey.value);
            auto loc_itr = loc_table.find(stringkey.value);
            eosio_assert(loc_itr != loc_table.end(), (string("String with key '") + stringkey.to_string() + "' does not exist").c_str());            
            
            // insert local for string
            uint64_t id = loc_table.available_primary_key();
            loc_table.emplace( owner, [&]( auto& s ) {
                s.id = id;
                s.text = text;
                s.local = local;
                s.version = version;
                s.registerer = registerer;
            });
        };

    private:
        // scope: contract
        // row: el registro de una entrada. Debe al menos tener una traducción en locals
        TABLE string_entry { 
            name              key;
            uint64_t   registerer; // vapaeeauthor.authors.id
            uint64_t primary_key() const { return key.value; }
            uint64_t secondary_key() const { return registerer; }
        };
        typedef eosio::multi_index<"strings"_n, string_entry, 
            indexed_by<"second"_n, const_mem_fun<string_entry, uint64_t, &string_entry::secondary_key>>
        > strings;

        // scope: string_entry.id
        TABLE local_entry { 
            uint64_t           id;
            string           text;
            name            local;
            name          version;
            uint64_t   registerer; // vapaeeauthor.authors.id
            uint64_t primary_key() const { return id; }
            uint64_t version_key() const { return local.value; }
            uint128_t secondary_key() const { return vapaee::combine(local, version); }
        };
        typedef eosio::multi_index<"locals"_n, local_entry, 
            indexed_by<"second"_n, const_mem_fun<local_entry, uint128_t, &local_entry::secondary_key>>,
            indexed_by<"version"_n, const_mem_fun<local_entry, uint64_t, &local_entry::version_key>>
        > locals;

};

}; // namespace
