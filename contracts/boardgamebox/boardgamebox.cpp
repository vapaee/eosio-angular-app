#include "boardgamebox.hpp"
using namespace eosio;

EOSIO_DISPATCH( vapaee::boardgamebox,
    (newapp)(registerapp)(newpublisher)(registeruser)(registerslug)(transferslug)(transferid)
    (newitem)(newcontainer)(newasset)(newinventory)(issueunits)(transfer)(transfunits)(newusercont)(newuser4app)(swap)(open)(close)(burn)
    (droptables)
)