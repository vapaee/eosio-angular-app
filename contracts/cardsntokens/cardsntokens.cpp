#include "cardsntokens.hpp"
using namespace eosio;

ACTION cardsntokens::hi( name user ) {
   print_f( "cardsntokens % from cardsntokens contract", user );
}

EOSIO_DISPATCH( cardsntokens, (hi) )