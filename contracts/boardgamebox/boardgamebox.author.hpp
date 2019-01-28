#pragma once
#include <vapaee/bgbox/common.hpp>

using namespace eosio;
namespace vapaee {
    namespace bgbox {

    class author {
        name _self;
    
    public:
        
        author():_self("boardgamebox"_n){}

        inline name get_self()const { return _self; }
  
        // APPS ---------------------------
        void action_new_app(name owner, name contract, string slugid, string title, int inventory) {
            print("action_new_app()\n");
            print("owner: ", owner.to_string(), "\n");
            print("slugid: ", slugid.c_str(), "\n");
            print("title: ", title.c_str(), "\n");
            print("inventory: ", inventory, "\n");
            print("contract: ", contract.to_string(), "\n");
            authors authors_table(get_self(), get_self().value);
            // recover created author's id
            uint64_t author_id = authors_table.available_primary_key();

            // recorremos por id
            // for (auto itr = authors_table.begin(); itr != authors_table.end(); ) {
            //     print("newapp() id: ", itr->to_string(), "\n");
            //     itr++;
            // }

            // slug auth_nick = vapaee::bgbox::get_author_nick(author_id);
            // slug given_nick(nickstr);
            // string error_str = string("INTERNAL ERROR: '") + given_nick.to_string() + "' != '" + auth_nick.to_string() + "' author_id: " + std::to_string((int)author_id);
            // eosio_assert(auth_nick == given_nick, error_str.c_str());

            // register nick for owner and generate Author ID
            action(
                permission_level{owner,"active"_n},
                get_self(),
                "registerslug"_n,
                std::make_tuple(owner, slugid)
            ).send();
            
            // call BG-box for app registration
            action(
                permission_level{owner,"active"_n},
                "boardgamebox"_n,
                "registerapp"_n,
                std::make_tuple(owner, contract, author_id, inventory, title)
            ).send();
        }

        // -- INCOMPLETO --
        void action_register_app(name author_owner, name app_contract, uint64_t author_app, int invespace, const string &title) {
            print("author::action_register_app()\n");
            print(" author_owner: ", author_owner.to_string(), "\n");
            print(" app_contract: ", app_contract.to_string(), "\n");
            print(" author_app: ", std::to_string((int) author_app), "\n");
            print(" invespace: ", std::to_string((int) invespace), "\n");
            print(" title: ", title, "\n");

            authors author_table(get_self(), get_self().value);
            auto author_itr = author_table.find(author_app);
            slug app_slug =  author_itr->slugid;
            eosio_assert(author_itr != author_table.end(), (string("author not found. id: ") + std::to_string((int) author_app)).c_str());
            
            // verifica que no exista un registro para el author_app
            apps app_table(get_self(), get_self().value);
            auto itr = app_table.find(author_app);
            eosio_assert(itr == app_table.end(), (string("author already registered as app. id: ") + std::to_string((int) author_app)).c_str());
            // crea un nuevo apps 
            app_table.emplace( author_owner, [&]( auto& s ) {
                s.id = author_app;
                s.contract = app_contract;
                s.title = title;
            });
            print(" app_table.emplace() new app regitered\n");


            // tengo que hallar el id del container inventory y del deposit porque los tengo que usar
            container_specs container_table(get_self(), get_self().value);
            uint64_t inv_id = container_table.available_primary_key();
            uint64_t dep_id = inv_id + 1;

            print(" next inv_id: ", std::to_string((int) inv_id), "\n");
            print(" next dep_id: ", std::to_string((int) dep_id), "\n");

            // inventory spec
            action(
                permission_level{author_owner,"active"_n},
                get_self(),
                "newcontainer"_n,
                std::make_tuple(author_owner, author_app, "inventory"_n, invespace)
            ).send();


            // deposit spec
            action(
                permission_level{author_owner,"active"_n},
                get_self(),
                "newcontainer"_n,
                std::make_tuple(author_owner, author_app, "deposit"_n, 0)
            ).send();


            
            // inventory asset
            slug inv_slug = slug(app_slug.to_string() + ".inventory");
            action(
                permission_level{author_owner,"active"_n},
                get_self(),
                "newinventory"_n,
                std::make_tuple(author_owner, author_app, inv_slug, invespace, inv_id)
            ).send();


            // deposit asset
            slug dep_slug = slug(app_slug.to_string() + ".deposit");
            action(
                permission_level{author_owner,"active"_n},
                get_self(),
                "newinventory"_n,
                std::make_tuple(author_owner, author_app, dep_slug, invespace, dep_id)
            ).send();
            

            print("author::action_register_app() ends...\n");
        }

        // PUBLISHERS ---------------------------
        void action_new_publisher(name owner, string slugid, string name) {
            print("action_new_publisher()\n");
            authors authors_table(get_self(), get_self().value);
            // recover created author's id
            uint64_t author_id = authors_table.available_primary_key();

            action(
                permission_level{owner,"active"_n},
                get_self(),
                "registerslug"_n,
                std::make_tuple(owner, slugid)
            ).send();

            // call BG-box for app registration
            action(
                permission_level{owner,"active"_n},
                "boardgamebox"_n,
                "registeruser"_n,
                std::make_tuple(owner, author_id, name)
            ).send();

            print("action_new_publisher() ends...\n");
        }

        // -- INCOMPLETO --
        void action_register_user(name owner, uint64_t author_id, const string &username) {
            print("author::action_register_user()\n");
            print(" owner: ", owner.to_string(), "\n");
            print(" author_id: ", std::to_string((int) author_id), "\n");
            print(" username: ", username, "\n");

            authors author_table(get_self(), get_self().value);
            auto author_itr = author_table.find(author_id);
            eosio_assert(author_itr != author_table.end(), (string("ERROR: author not found. id: ") + std::to_string((int) author_id)).c_str());
            eosio_assert(owner == author_itr->owner,
                (string("ERROR: owner '" + owner.to_string() + "' differs from author current owner '" + author_itr->owner.to_string() + "'")).c_str());
            
            // verifica que no exista un registro para el author_id
            users user_table(get_self(), get_self().value);
            auto itr = user_table.find(author_id);
            eosio_assert(itr == user_table.end(), (string("ERROR: author already registered as user. id: ") + std::to_string((int) author_id)).c_str());
            // crea un nuevo user 
            user_table.emplace( owner, [&]( auto& s ) {
                s.id = author_id;
                s.username = username;
            });
            print(" user_table.emplace() new user ", username, " regitered\n" );

            print("author::action_register_user() ends...\n");
        }        

        void action_register_slug(name owner, string slugidstr) {
            print("author::action_register_nick()\n");
            
            print(" owner: ", owner.to_string(), "\n");
            print(" slugid: ", slugidstr.c_str(), "\n");
            authors authors_table(get_self(), get_self().value);

            require_auth( owner );
            slug slugid(slugidstr);
            
            auto index = authors_table.template get_index<"slugid"_n>();
            auto itr = index.find( slugid.to128bits() );

            while (itr != index.end() && itr->slugid != slugid) {
                itr++;
            }

            eosio_assert(itr == index.end(), (string("slugid: '") + slugid.to_string() + "' already registered by " + itr->owner.to_string()).c_str() );
            uint64_t author_id = authors_table.available_primary_key();
            authors_table.emplace( owner, [&]( auto& s ) {
                s.id            = author_id;
                s.owner         = owner;
                s.ownervalue    = owner.value;
                s.slugid        = slugid;
                s.slugvalue     = slugid.to128bits();
            });

            // recorremos por id
            for (auto itr = authors_table.begin(); itr != authors_table.end(); ) {
                print("registerslugid() id: ", itr->to_string(), "\n");
                itr++;
            }

            vapaee::bgbox::get_author_slug(author_id);

            print("author::action_register_slugid() ends...\n");
        }

        void action_transfer_by_slug(name owner, name newowner, slug nick) {
            print("author::action_transfer_by_nick()\n");
        }

        void action_transfer_by_id(name owner, name newowner, uint64_t id) {
            print("author::action_transfer_by_id()\n");
        }

    }; // class

}; // namespace

}; // namespace