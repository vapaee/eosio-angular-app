#!/bin/bash

cleos push action vapaeetokens configfee '["create","1.0000 ACORN"]' -p vapaeetokens@active

echo "-- deposits --"
cleos push action eosio.token transfer  '["bob",  "vapaeetokens","10.0000 ACORN","deposit"]' -p bob@active
cleos push action eosio.token transfer  '["alice","vapaeetokens","10.0000 ACORN","deposit"]' -p alice@active
# cleos push action eosio.token transfer  '["tom",  "vapaeetokens","10.0000 ACORN","deposit"]' -p tom@active
# cleos push action eosio.token transfer  '["kate", "vapaeetokens","10.0000 ACORN","deposit"]' -p kate@active

cleos push action vapaeetokens transfer '["bob",  "vapaeetokens","100.0000 CNT","deposit"]' -p bob@active
cleos push action vapaeetokens transfer '["alice","vapaeetokens","100.0000 CNT","deposit"]' -p alice@active
# cleos push action vapaeetokens transfer '["tom","vapaeetokens","0.1000 CNT","deposit"]' -p tom@active
# cleos push action vapaeetokens transfer '["kate","vapaeetokens","9.9999 CNT","deposit"]' -p kate@active

cleos push action eosio.token transfer  '["bob",  "vapaeetokens","100.0000 TLOS","deposit"]' -p bob@active
cleos push action eosio.token transfer  '["alice","vapaeetokens","100.0000 TLOS","deposit"]' -p alice@active

### echo "-- 3 orders --"
### cleos push action vapaeetokens order '["alice","buy", "5.0000 CNT", "0.2000 TLOS", "1.0000 TLOS"]' -p alice
### cleos push action vapaeetokens order '["alice","buy","10.0000 CNT", "0.1000 TLOS", "1.0000 TLOS"]' -p alice
### cleos push action vapaeetokens order '["alice","buy", "2.5000 CNT", "0.4000 TLOS", "1.0000 TLOS"]' -p alice
### 
### echo "-- 1 buy and 2 cancels --"
### cleos push action vapaeetokens order '["bob","sell",  "2.5000 CNT", "0.4000 TLOS", "2.5000 CNT"]' -p bob
### cleos push action vapaeetokens cancel '["alice", "sell", "TLOS", "CNT", [0,1]]' -p alice
### 
### sleep 1
### 
### echo "-- 3 same orders --"
### cleos push action vapaeetokens order '["alice","buy", "5.0000 CNT", "0.2000 TLOS", "1.0000 TLOS"]' -p alice
### cleos push action vapaeetokens order '["alice","buy","10.0000 CNT", "0.1000 TLOS", "1.0000 TLOS"]' -p alice
### cleos push action vapaeetokens order '["alice","buy", "2.5000 CNT", "0.4000 TLOS", "1.0000 TLOS"]' -p alice
### 
### echo "-- 1 buy and overflows --"
### cleos push action vapaeetokens order '["bob","sell", "5.0000 CNT", "0.4000 TLOS", "5.0000 CNT"]' -p bob
### cleos push action vapaeetokens cancel '["bob", "sell", "CNT", "TLOS", [0]]' -p bob
### cleos push action vapaeetokens cancel '["alice", "sell", "TLOS", "CNT", [0,1]]' -p alice

cleos push action vapaeetokens order '["alice","buy", "5.0000 CNT", "0.2000 TLOS", "1.0000 TLOS"]' -p alice
cleos push action vapaeetokens order '["alice","buy","10.0000 CNT", "0.1000 TLOS", "1.0000 TLOS"]' -p alice
cleos push action vapaeetokens order '["alice","buy", "2.5000 CNT", "0.4000 TLOS", "1.0000 TLOS"]' -p alice

cleos push action vapaeetokens order '["bob","sell",  "7.5000 CNT", "0.1000 TLOS", "7.5000 CNT"]' -p bob



# cleos push action vapaeetokens withdraw '["alice", "9.0000 ACORN"]' -p alice
# cleos push action vapaeetokens withdraw '["bob", "4.0000 ACORN"]' -p bob

# cleos push action eosio.token transfer '["alice","vapaeetokens","8.0000 ACORN","deposit"]' -p alice@active
# cleos push action eosio.token transfer '["bob","vapaeetokens","3.0000 ACORN","deposit"]' -p bob@active