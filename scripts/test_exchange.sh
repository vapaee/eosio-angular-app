#!/bin/bash

cleos push action vapaeetokens configfee '["create","1.0000 ACORN"]' -p vapaeetokens@active
cleos push action eosio.token transfer '["bob","vapaeetokens","10.0000 ACORN","deposit"]' -p bob@active
cleos push action eosio.token transfer '["alice","vapaeetokens","10.0000 ACORN","deposit"]' -p alice@active
cleos push action eosio.token transfer '["tom","vapaeetokens","10.0000 ACORN","deposit"]' -p tom@active
cleos push action eosio.token transfer '["kate","vapaeetokens","10.0000 ACORN","deposit"]' -p kate@active

cleos push action eosio.token transfer '["alice", "vapaeetokens", "1.0000 TLOS", "buy|5.0000 CNT|0.2000 TLOS"]' -p alice
cleos push action eosio.token transfer '["alice", "vapaeetokens", "1.0000 TLOS", "buy|10.0000 CNT|0.1000 TLOS"]' -p alice
cleos push action eosio.token transfer '["alice", "vapaeetokens", "1.0000 TLOS", "buy|2.5000 CNT|0.4000 TLOS"]' -p alice

cleos push action vapaeetokens transfer '["bob", "vapaeetokens", "2.5000 CNT", "sell|2.5000 CNT|0.4000 TLOS"]' -p bob
cleos push action vapaeetokens cancel '["alice", "sell", "TLOS", "CNT", [0,1]]' -p alice

cleos push action vapaeetokens retire '["alice", "9.0000 ACORN"]' -p alice
cleos push action vapaeetokens retire '["bob", "4.0000 ACORN"]' -p bob

cleos push action eosio.token transfer '["alice","vapaeetokens","8.0000 ACORN","deposit"]' -p alice@active
cleos push action eosio.token transfer '["bob","vapaeetokens","3.0000 ACORN","deposit"]' -p bob@active