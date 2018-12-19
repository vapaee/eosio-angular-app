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
            require_auth( owner );
            slug nick(nickstr);
            print("owner: ", owner.to_string(), "\n");
            print("nick: ", nick.to_string(), "\n");
            
            auto index = this->authors.template get_index<"nick"_n>();
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
            // https://eosio.stackexchange.com/questions/1214/delete-all-multi-index-records-without-iterator
            // para poder desarrollar más fácil            
            
            // recorremos por id
            for (auto itr = this->authors.begin(); itr != this->authors.end(); ) {
                print("id", itr->to_string(), "\n");
                itr++;
                // itr = this->authors.erase(itr);
            }

            // recorremos por owner
            auto index_owner = this->authors.template get_index<"owner"_n>();
            for (auto itr = index_owner.begin(); itr != index_owner.end(); ) {
                print("owner", itr->to_string(), "\n");
                itr++;
                // itr = index_nick.erase(itr);
            }

            // recorremos por slug
            auto index_nick = this->authors.template get_index<"nick"_n>();
            for (auto itr = index_nick.begin(); itr != index_nick.end(); ) {
                print("nick", itr->to_string(), "\n");
                itr++;
                // itr = index_nick.erase(itr);
            }

                     
            // borro las filas ----
            for (auto itr = this->authors.begin(); itr != this->authors.end(); ) {
                print("id", itr->to_string(), "\n");
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
            indexed_by<"nick"_n, const_mem_fun<author_slug, uint128_t, &author_slug::secondary_key>>
        > author_slugs ;
        
        author_slugs authors;
        
};

}; // namespace