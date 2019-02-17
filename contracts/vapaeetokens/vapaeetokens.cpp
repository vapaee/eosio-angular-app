#include "vapaeetokens.hpp"

#define TOKEN_ACTIONS (create)(addissuer)(removeissuer)(issue)(transfer)(open)(close)(burn)
#define AIRDROP_ACTIONS (setsnapshot)(nosnapshot)(claim)
#define EXCHANGE_ACTIONS (addtoken)(cancel)
#define STAKE_ACTIONS (stake)(unstake)(restake)(unstakeback)(unstaketime)

#define EXCHANGE_HANDLERS (htransfer)

EOSIO_DISPATCH_VAPAEETOKENS(
    vapaee::vapaeetokens,
    TOKEN_ACTIONS AIRDROP_ACTIONS EXCHANGE_ACTIONS STAKE_ACTIONS,
    EXCHANGE_HANDLERS
    )