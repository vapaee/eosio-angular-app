#include "hello.hpp"
using namespace eosio;

ACTION hello::hi( name user ) {
   print_f( "Hello % from hello contract", user );
}

EOSIO_DISPATCH( hello, (hi) )