#include "vapaeetokens.hpp"
using namespace eosio;

ACTION vapaeetokens::hi( name user ) {
   print_f( "vapaeetokens % from vapaeetokens contract", user );
}

EOSIO_DISPATCH( vapaeetokens, (hi) )