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
  
        void action_new_app(name owner, name contract, string slugid, string title, int inventory) {
            print("action_new_app()\n");
            print("owner: ", owner.to_string(), "\n");
            print("slugid: ", slugid.c_str(), "\n");
            print("title: ", title.c_str(), "\n");
            print("inventory: ", inventory, "\n");
            print("contract: ", contract.to_string(), "\n");
            authors authors_table(get_self(), get_self().value);
            // recover created author's id
            uint64_t app_author_id = authors_table.available_primary_key();

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
                get_self(),
                "registerapp"_n,
                std::make_tuple(owner, contract, app_author_id, inventory, title)
            ).send();
        }
        
        void action_register_app(name owner, name app_contract, uint64_t app_author_id, int invespace, const string &title) {
            print("author::action_register_app()\n");
            print(" owner: ", owner.to_string(), "\n");
            print(" app_contract: ", app_contract.to_string(), "\n");
            print(" app_author_id: ", std::to_string((int) app_author_id), "\n");
            print(" invespace: ", std::to_string((int) invespace), "\n");
            print(" title: ", title, "\n");

            // verifies app_author_id exists in authors table
            authors author_table(get_self(), get_self().value);
            auto author_itr = author_table.find(app_author_id);
            slug app_slugid =  author_itr->slugid;
            eosio_assert(author_itr != author_table.end(), (string("author not found. id: ") + std::to_string((int) app_author_id)).c_str());
            
            // verifies app_author_id does not exists in apps table
            apps app_table(get_self(), get_self().value);
            auto itr = app_table.find(app_author_id);
            eosio_assert(itr == app_table.end(), (string("author already registered as app. id: ") + std::to_string((int) app_author_id)).c_str());

            // creates a new app
            app_table.emplace( owner, [&]( auto& s ) {
                s.id = app_author_id;
                s.contract = app_contract;
                s.title = title;
            });
            print(" apps.emplace() new app registered\n");


            // We obtain the id of the inventory and deposit containers spec because we are going to need them to create the container assets
            container_specs container_table(get_self(), get_self().value);
            uint64_t inv_id = container_table.available_primary_key();
            uint64_t dep_id = inv_id + 1;

            print(" next inv_id: ", std::to_string((int) inv_id), "\n");
            print(" next dep_id: ", std::to_string((int) dep_id), "\n");

            // -- container specs --
            // inventory spec
            action(
                permission_level{owner,"active"_n},
                get_self(),
                "newcontainer"_n,
                std::make_tuple(owner, app_author_id, "inventory"_n)
            ).send();


            // deposit spec
            action(
                permission_level{owner,"active"_n},
                get_self(),
                "newcontainer"_n,
                std::make_tuple(owner, app_author_id, "deposit"_n)
            ).send();


            // -- container assets --
            // inventory asset
            slug inv_slug = slug(app_slugid.to_string() + ".inventory");
            action(
                permission_level{owner,"active"_n},
                get_self(),
                "newinventory"_n,
                std::make_tuple(owner, app_author_id, inv_slug, invespace, inv_id)
            ).send();


            // deposit asset
            slug dep_slug = slug(app_slugid.to_string() + ".deposit");
            action(
                permission_level{owner,"active"_n},
                get_self(),
                "newinventory"_n,
                std::make_tuple(owner, app_author_id, dep_slug, 0, dep_id)
            ).send();
            

            print("author::action_register_app() ends...\n");
        }
       
        void action_new_profile(name owner, string slugid, string name) {
            print("action_new_profile()\n");
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
                get_self(),
                "registerprof"_n,
                std::make_tuple(owner, author_id, name)
            ).send();

            print("action_new_profile() ends...\n");
        }

        void action_register_profile(name owner, uint64_t author_id, const string &username) {
            print("author::action_register_profile()\n");
            print(" owner: ", owner.to_string(), "\n");
            print(" author_id: ", std::to_string((int) author_id), "\n");
            print(" username: ", username, "\n");

            // verifies author_id exists in authors table
            authors author_table(get_self(), get_self().value);
            auto author_itr = author_table.find(author_id);
            eosio_assert(author_itr != author_table.end(), (string("ERROR: author not found. id: ") + std::to_string((int) author_id)).c_str());
            eosio_assert(owner == author_itr->owner,
                (string("ERROR: owner '" + owner.to_string() + "' differs from author current owner '" + author_itr->owner.to_string() + "'")).c_str());
            
            // verifies author_id does not exists in profiles table
            profiles profile_table(get_self(), get_self().value);
            auto itr = profile_table.find(author_id);
            eosio_assert(itr == profile_table.end(), (string("ERROR: author already registered as user. id: ") + std::to_string((int) author_id)).c_str());
            
            // creates a new profile
            profile_table.emplace( owner, [&]( auto& s ) {
                s.id = author_id;
                s.username = username;
            });

            print(" profiles.emplace() new user ", username, " regitered\n" );

            print("author::action_register_profile() ends...\n");
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
                s.slugid        = slugid;
                s.ownervalue    = owner.value;        // temp - for debugging porpuses
                s.slugvalue     = slugid.to128bits(); // temp - for debugging porpuses
            });

            // printing all registered authors (debugging)
            for (auto itr = authors_table.begin(); itr != authors_table.end(); ) {
                print("registerslugid() id: ", itr->to_string(), "\n");
                itr++;
            }

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