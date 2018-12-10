#include "boardbagebox.hpp"
using namespace eosio;

ACTION boardbagebox::hi( name user ) {
   print_f( "boardbagebox % from boardbagebox contract", user );
}

EOSIO_DISPATCH( boardbagebox, (hi) )