#include <eosiolib/eosio.hpp>
#include "my_datatype.hpp"

using namespace eosio;

CONTRACT hello : public eosio::contract {
    public:
        using contract::contract;

        ACTION trydata( my_datatype data) {
            // this doesn't work
        }
        ACTION trystr( string str) {
            my_datatype data(str);
            // this works fine
        }


        
};

EOSIO_DISPATCH( hello, (trydata)(trystr) )
























/*

#include <eosiolib/eosio.hpp>
#include "../includes/vapaee/base/slug1.hpp"

using namespace eosio;
using namespace vapaee;

CONTRACT hello : public eosio::contract {
  public:
      using contract::contract;

      ACTION hi( string user  ) {
          slug a(user);
          print("HELLO: " + a.to_string());
      }
};

EOSIO_DISPATCH( hello, (hi) )

*/