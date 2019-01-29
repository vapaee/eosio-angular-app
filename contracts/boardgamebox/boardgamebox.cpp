#include "boardgamebox.hpp"
using namespace eosio;

EOSIO_DISPATCH( vapaee::boardgamebox,
    (newapp)(registerapp)(newprofile)(registerprof)(registerslug)(transferslug)(transferid)
    (newitem)(newcontainer)(newasset)(newinventory)(issueunits)(transfer)(transfunits)(cont4profile)(profile4app)(swap)(open)(close)(burn)
    (droptables)
)