#!/bin/bash
# rm /var/www/blockchain/eosio/data -fr

# cleos wallet import
# eosio: 5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3
# cleos wallet import
# dev: 5J1astpVJcAJVGX8PGWN9KCcHU5DMszi4gJgCEpWc5DxmpTsKqp

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

cleos set account permission cardsntokens active '{"threshold": 1,"keys": [{"key": "EOS8RoCAXxWYUW2v4xkG19F57BDVBzpt9NN2iDsD1ouQNyV2BkiNc","weight": 1}],"accounts": [{"permission":{"actor":"cardsntokens","permission":"eosio.code"},"weight":1}]}' owner -p cardsntokens
cleos set account permission vapaeetokens active '{"threshold": 1,"keys": [{"key": "EOS8RoCAXxWYUW2v4xkG19F57BDVBzpt9NN2iDsD1ouQNyV2BkiNc","weight": 1}],"accounts": [{"permission":{"actor":"vapaeetokens","permission":"eosio.code"},"weight":1}]}' owner -p vapaeetokens
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

echo "-------- eosio.token (ACORN) ---------"
cleos push action eosio.token create '[ "eosio", "1000000000.0000 ACORN"]' -p eosio.token@active
cleos push action eosio.token issue '["alice", "10.0000 ACORN", "memo ACORN"]' -p eosio@active
cleos push action eosio.token issue '["bob", "10.0000 ACORN", "memo ACORN"]' -p eosio@active
cleos push action eosio.token issue '["tom", "10.0000 ACORN", "memo ACORN"]' -p eosio@active
cleos push action eosio.token issue '["kate", "10.0000 ACORN", "memo ACORN"]' -p eosio@active

echo "-------- eosio.token (VIITA) ---------"
cleos push action eosio.token create '[ "eosio", "10000000000.0000 VIITA"]' -p eosio.token@active
cleos push action eosio.token issue '["alice", "10.0000 VIITA", "memo VIITA"]' -p eosio@active
cleos push action eosio.token issue '["bob", "10.0000 VIITA", "memo VIITA"]' -p eosio@active
cleos push action eosio.token issue '["tom", "10.0000 VIITA", "memo VIITA"]' -p eosio@active
cleos push action eosio.token issue '["kate", "10.0000 VIITA", "memo VIITA"]' -p eosio@active


echo "-------- eosio.token (QBE) ---------"
cleos push action eosio.token create '[ "eosio", "100000000.0000 QBE"]' -p eosio.token@active
cleos push action eosio.token issue '["alice", "10.0000 QBE", "memo QBE"]' -p eosio@active
cleos push action eosio.token issue '["bob", "10.0000 QBE", "memo QBE"]' -p eosio@active
cleos push action eosio.token issue '["tom", "10.0000 QBE", "memo QBE"]' -p eosio@active
cleos push action eosio.token issue '["kate", "10.0000 QBE", "memo QBE"]' -p eosio@active


echo "-------- eosio.token (CNT) ---------"
cleos push action eosio.token create '[ "eosio", "500000000.0000 CNT"]' -p eosio.token@active
cleos push action eosio.token issue '["alice", "10.0000 CNT", "memo CNT"]' -p eosio@active
cleos push action eosio.token issue '["bob", "10.0000 CNT", "memo CNT"]' -p eosio@active
cleos push action eosio.token issue '["tom", "10.0000 CNT", "memo CNT"]' -p eosio@active
cleos push action eosio.token issue '["kate", "10.0000 CNT", "memo CNT"]' -p eosio@active


echo "-------- eosio.token (EDNA) ---------"
cleos push action eosio.token create '[ "eosio", "1300000000.0000 EDNA"]' -p eosio.token@active
cleos push action eosio.token issue '["alice", "10.0000 EDNA", "memo EDNA"]' -p eosio@active
cleos push action eosio.token issue '["bob", "10.0000 EDNA", "memo EDNA"]' -p eosio@active
cleos push action eosio.token issue '["tom", "10.0000 EDNA", "memo EDNA"]' -p eosio@active
cleos push action eosio.token issue '["kate", "10.0000 EDNA", "memo EDNA"]' -p eosio@active


echo "-------- eosio.token (TEACH) ---------"
cleos push action eosio.token create '[ "eosio", "10000000000.0000 TEACH"]' -p eosio.token@active
cleos push action eosio.token issue '["alice", "10.0000 TEACH", "memo TEACH"]' -p eosio@active
cleos push action eosio.token issue '["bob", "10.0000 TEACH", "memo TEACH"]' -p eosio@active
cleos push action eosio.token issue '["tom", "10.0000 TEACH", "memo TEACH"]' -p eosio@active
cleos push action eosio.token issue '["kate", "10.0000 TEACH", "memo TEACH"]' -p eosio@active


echo "-------- eosio.token (ROBO) ---------"
cleos push action eosio.token create '[ "eosio", "1000000000.0000 ROBO"]' -p eosio.token@active
cleos push action eosio.token issue '["alice", "10.0000 ROBO", "memo ROBO"]' -p eosio@active
cleos push action eosio.token issue '["bob", "10.0000 ROBO", "memo ROBO"]' -p eosio@active
cleos push action eosio.token issue '["tom", "10.0000 ROBO", "memo ROBO"]' -p eosio@active
cleos push action eosio.token issue '["kate", "10.0000 ROBO", "memo ROBO"]' -p eosio@active


# snapshot
