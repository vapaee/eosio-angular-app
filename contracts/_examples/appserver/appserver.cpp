#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;

CONTRACT appserver : public eosio::contract {

    public:
        using contract::contract;
      
        appserver(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {}
      
        ACTION inside(name signatory, std::string message) {
            print("\nACTION appserver.inside() signatory: ", signatory, " msg: ", message.c_str(), "\n");
            require_auth(signatory);

            action(
                permission_level{get_self(),"active"_n},
                get_self(),
                "notify"_n,
                std::make_tuple(signatory, message)
            ).send();
        }

        ACTION notify(name signatory, std::string message) {
            print("\nACTION appserver.notify() signatory: ", signatory, " msg: ", message.c_str(), "\n");
            require_auth(get_self());
            require_recipient(signatory);
        }

        // -----------------------

        ACTION outside(name signatory, std::string message) {
            print("\nACTION appserver.outside() signatory: ", signatory, " msg: ", message.c_str(), "\n");
            require_auth(signatory);

            action(
                permission_level{get_self(),"active"_n},
                "appplugin"_n,
                "notify"_n,
                std::make_tuple(signatory, signatory.to_string() + message)
            ).send();
        }        
 
};

EOSIO_DISPATCH( appserver, (inside)(notify)(outside))