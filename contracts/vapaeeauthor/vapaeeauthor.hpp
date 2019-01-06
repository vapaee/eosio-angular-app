#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <vapaee/slug.hpp>
#include <vapaee/vapaee_utils.hpp>

using namespace eosio;
namespace vapaee {

CONTRACT vapaeeauthor : public eosio::contract {
    public:

        using contract::contract;

        vapaeeauthor(name receiver, name code, datastream<const char*> ds):
            contract(receiver,code,ds),authors(receiver, receiver.value)
        {}

        ACTION newapp(name owner, name contract, string nickstr, string title, int inventory) {
            print("\nACTION newapp()\n");
            print("owner: ", owner.to_string(), "\n");
            print("nickstr: ", nickstr.c_str(), "\n");
            print("title: ", title.c_str(), "\n");
            print("inventory: ", inventory, "\n");
            print("contract: ", contract.to_string(), "\n");
            // recover created author's id
            uint64_t author_id = this->authors.available_primary_key();

            // recorremos por id
            // for (auto itr = this->authors.begin(); itr != this->authors.end(); ) {
            //     print("newapp() id: ", itr->to_string(), "\n");
            //     itr++;
            // }

            // slug auth_nick = vapaee::get_author_nick(author_id);
            // slug given_nick(nickstr);
            // string error_str = string("INTERNAL ERROR: '") + given_nick.to_string() + "' != '" + auth_nick.to_string() + "' author_id: " + std::to_string((int)author_id);
            // eosio_assert(auth_nick == given_nick, error_str.c_str());

            // register nick with owner and contract names
            action(
                permission_level{owner,"active"_n},
                get_self(),
                "registernick"_n,
                std::make_tuple(owner, contract, nickstr, title)
            ).send();
            
            // call BG-box for app registration
            action(
                permission_level{get_self(),"active"_n},
                "boardgamebox"_n,
                "newapp"_n,
                std::make_tuple(owner, contract, author_id, inventory)
            ).send();
            
        }

        ACTION newpublisher(name owner, string nickstr, string title) {
            print("\nACTION newpublisher()\n");

            action(
                permission_level{owner,"active"_n},
                get_self(),
                "registernick"_n,
                std::make_tuple(owner, get_self(), nickstr, title)
            ).send();

            // recorremos por id
            for (auto itr = this->authors.begin(); itr != this->authors.end(); ) {
                print("newpublisher() id: ", itr->to_string(), "\n");
                itr++;
            }            
        }        


        ACTION registernick(name owner, name contract, string nickstr, string title) {
            print("\nACTION registernick()\n");
            print("owner: ", owner.to_string(), "\n");
            print("nickstr: ", nickstr.c_str(), "\n");
            print("title: ", title.c_str(), "\n");
            print("contract: ", contract.to_string(), "\n");

            require_auth( owner );
            slug nick(nickstr);
            
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
                s.title         = title;
                s.contract      = contract;
            });

            // recorremos por id
            for (auto itr = this->authors.begin(); itr != this->authors.end(); ) {
                print("registernick() id: ", itr->to_string(), "\n");
                itr++;
            }
        }

        ACTION transfernick(name owner, name newowner, slug nick) {
            print("\nACTION transfernick()\n");
        }

        ACTION transferid(name owner, name newowner, uint64_t id) {
            print("\nACTION transferid()\n");
        }

        ACTION droptables() {
            print("\nACTION droptables()\n");
            // https://eosio.stackexchange.com/questions/1214/delete-all-multi-index-records-without-iterator
            // para poder desarrollar más fácil            
            
            /*
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
            */

                     
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
        
        author_slugs authors;
        
};

}; // namespace