#!/bin/bash


cleos push action snapsnapsnap add '["bob",250000000,1]' -p bob@active
cleos push action snapsnapsnap add '["alice",10000000000,1]' -p alice@active
cleos push action snapsnapsnap add '["tom",1000,1]' -p tom@active
cleos push action snapsnapsnap add '["kate",9999999,1]' -p kate@active


# register TLOS token 
cleos push action vapaeetokens addtoken '["eosio.token","TLOS","vapaeetokens"]' -p vapaeetokens@active

cleos push action vapaeetokens create '["vapaeetokens","500000000.0000 BOX"]' -p vapaeetokens@active
cleos push action vapaeetokens create '["vapaeetokens","500000000.0000 CNT"]' -p vapaeetokens@active
cleos push action vapaeetokens create '["vapaeetokens","1000000.0000 VPE"]' -p vapaeetokens@active

# ratio 1:1 - min 1000.0000 TLOS - cap 40000.0000 TLOS
cleos push action vapaeetokens setsnapshot '["snapsnapsnap",1,"BOX",400000000,10000000,1000,0]' -p vapaeetokens@active

# ratio 1:1
cleos push action vapaeetokens setsnapshot '["snapsnapsnap",1,"CNT",0,0,10000,0]' -p vapaeetokens@active

# ratio 1:0 - min 1000.0000 TLOS - base 1.0000 VPE
cleos push action vapaeetokens setsnapshot '["snapsnapsnap",1,"VPE",0,10000000,0,10000]' -p vapaeetokens@active


# add issuers to the tokens
cleos push action vapaeetokens addissuer '["boardgamebox","BOX"]' -p vapaeetokens@active
cleos push action vapaeetokens addissuer '["cardsntokens","CNT"]' -p vapaeetokens@active
cleos push action vapaeetokens addissuer '["vapaeetokens","VPE"]' -p vapaeetokens@active

