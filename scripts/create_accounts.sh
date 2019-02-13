#!/bin/bash
# rm /var/www/blockchain/eosio/data -fr

cleos create account eosio gqydoobuhege EOS8RoCAXxWYUW2v4xkG19F57BDVBzpt9NN2iDsD1ouQNyV2BkiNc
cleos create account eosio eosio.token EOS8RoCAXxWYUW2v4xkG19F57BDVBzpt9NN2iDsD1ouQNyV2BkiNc
cleos create account eosio cardsntokens EOS8RoCAXxWYUW2v4xkG19F57BDVBzpt9NN2iDsD1ouQNyV2BkiNc
cleos create account eosio vapaeetokens EOS8RoCAXxWYUW2v4xkG19F57BDVBzpt9NN2iDsD1ouQNyV2BkiNc
cleos create account eosio snapsnapsnap EOS8RoCAXxWYUW2v4xkG19F57BDVBzpt9NN2iDsD1ouQNyV2BkiNc
cleos create account eosio boardgamebox EOS8RoCAXxWYUW2v4xkG19F57BDVBzpt9NN2iDsD1ouQNyV2BkiNc
cleos create account eosio localstrings EOS8RoCAXxWYUW2v4xkG19F57BDVBzpt9NN2iDsD1ouQNyV2BkiNc

cleos create account eosio bob EOS8RoCAXxWYUW2v4xkG19F57BDVBzpt9NN2iDsD1ouQNyV2BkiNc
cleos create account eosio alice EOS8RoCAXxWYUW2v4xkG19F57BDVBzpt9NN2iDsD1ouQNyV2BkiNc
cleos create account eosio tom EOS8RoCAXxWYUW2v4xkG19F57BDVBzpt9NN2iDsD1ouQNyV2BkiNc
cleos create account eosio kate EOS8RoCAXxWYUW2v4xkG19F57BDVBzpt9NN2iDsD1ouQNyV2BkiNc

cleos create account eosio abcounter EOS8RoCAXxWYUW2v4xkG19F57BDVBzpt9NN2iDsD1ouQNyV2BkiNc
cleos create account eosio addressbook EOS8RoCAXxWYUW2v4xkG19F57BDVBzpt9NN2iDsD1ouQNyV2BkiNc
cleos create account eosio hello EOS8RoCAXxWYUW2v4xkG19F57BDVBzpt9NN2iDsD1ouQNyV2BkiNc
cleos create account eosio bounds EOS8RoCAXxWYUW2v4xkG19F57BDVBzpt9NN2iDsD1ouQNyV2BkiNc

cleos create account eosio appplugin EOS8RoCAXxWYUW2v4xkG19F57BDVBzpt9NN2iDsD1ouQNyV2BkiNc
cleos create account eosio appserver EOS8RoCAXxWYUW2v4xkG19F57BDVBzpt9NN2iDsD1ouQNyV2BkiNc

# cleos set account permission vapaeeauthor active '{"threshold": 1,"keys": [{"key": "EOS8RoCAXxWYUW2v4xkG19F57BDVBzpt9NN2iDsD1ouQNyV2BkiNc","weight": 1}],"accounts": [{"permission":{"actor":"vapaeeauthor","permission":"eosio.code"},"weight":1}]}' owner -p vapaeeauthor
cleos set account permission boardgamebox active '{"threshold": 1,"keys": [{"key": "EOS8RoCAXxWYUW2v4xkG19F57BDVBzpt9NN2iDsD1ouQNyV2BkiNc","weight": 1}],"accounts": [{"permission":{"actor":"boardgamebox","permission":"eosio.code"},"weight":1}]}' owner -p boardgamebox
cleos set account permission appserver active '{"threshold": 1,"keys": [{"key": "EOS8RoCAXxWYUW2v4xkG19F57BDVBzpt9NN2iDsD1ouQNyV2BkiNc","weight": 1}],"accounts": [{"permission":{"actor":"appserver","permission":"eosio.code"},"weight":1}]}' owner -p appserver

# eosio.token
echo "-------- eosio.token (TLOS) ---------"
cd /var/www/eosio-angular-app/contracts/_examples/eosio.contracts/eosio.token
if [[ src/eosio.token.cpp -nt eosio.token.wasm ]]; then
    eosio-cpp -o eosio.token.wasm src/eosio.token.cpp --abigen -I include
fi
cleos set contract eosio.token $PWD -p eosio.token@active
cleos push action eosio.token create '[ "eosio", "1000000000.0000 TLOS"]' -p eosio.token@active
cleos push action eosio.token issue '["alice", "120.0000 TLOS", "memo 120 TLOS"]' -p eosio@active
cleos push action eosio.token issue '["bob", "100.0000 TLOS", "memo 100 TLOS"]' -p eosio@active
cleos push action eosio.token issue '["tom", "0.1234 TLOS", "memo 0 TLOS"]' -p eosio@active
cleos push action eosio.token issue '["kate", "1.0000 TLOS", "memo 1 TLOS"]' -p eosio@active


# snapshot
