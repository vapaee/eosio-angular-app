#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <vapaee/slug.hpp>

using namespace eosio;
namespace vapaee {

CONTRACT vapaeeauthor : public eosio::contract {
    public:

        using contract::contract;

        vapaeeauthor(name receiver, name code, datastream<const char*> ds):
            contract(receiver,code,ds),authors(receiver, receiver.value)
        {}


        ACTION registernick(name owner, string nickstr) {
            // print(string(" owner: ") + owner.to_string());
            // print(string(" nick: ") + nick.to_string());
            slug nick(nickstr);
            print("owner: ", owner.to_string());
            print("nick: ", nick.to_string());
            
            
            auto index = this->authors.template get_index<"second"_n>();
            auto itr = index.find( nick.to128bits() );

            while (itr != index.end() && itr->nick != nick) {
                itr++;
            }

            eosio_assert(itr == index.end(), (string("nick: '") + nick.to_string() + "' already registered by " + itr->owner.to_string()).c_str() );

            this->authors.emplace( owner, [&]( auto& s ) {
                s.id            = this->authors.available_primary_key();
                s.owner         = owner;
                s.nick          = nick;
            });
            
        }

        ACTION transfernick(name owner, name newowner, slug nick) {
            
        }

        ACTION transferid(name owner, name newowner, uint64_t id) {
            
        }

        ACTION droptables() {
            // para poder desarrollar más fácil            
            
            for (auto itr = this->authors.begin(); itr != this->authors.end(); ) {
                print(itr->to_string());
                itr = this->authors.erase(itr);
            }
            
        };

    private:
        // scope: self
        
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
            indexed_by<"second"_n, const_mem_fun<author_slug, uint128_t, &author_slug::secondary_key>>
        > author_slugs ;
        
        author_slugs authors;
        
};

}; // namespace