#include "vapaeetokens.hpp"

#define TOKEN_ACTIONS (create)(issue)(transfer)(open)(close)(retire)
#define AIRDROP_ACTIONS (setsnapshot)(nosnapshot)(claim)
#define EXCHANGE_ACTIONS (addtoken)
#define STAKE_ACTIONS (stake)(unstake)(restake)(unstakeback)(unstaketime)

EOSIO_DISPATCH( vapaee::vapaeetokens, TOKEN_ACTIONS AIRDROP_ACTIONS EXCHANGE_ACTIONS STAKE_ACTIONS )