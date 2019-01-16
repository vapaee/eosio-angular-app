#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;

CONTRACT appplugin : public eosio::contract {
    public:
        using contract::contract;

        appplugin(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {}
    
        ACTION notify(name user, std::string message) {
            print("appplugin.notify() user: ", user, "message: ", message.c_str(), "\n");
            require_auth("appserver"_n);
        }
};

EOSIO_DISPATCH( appplugin, (notify));
