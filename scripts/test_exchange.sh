#!/bin/bash

cleos push action vapaeetokens configfee '["create","1.00000000 ACORN", 1]' -p vapaeetokens@active
cleos push action vapaeetokens configfee '["create","0.10000000 TLOS", 2]' -p vapaeetokens@active

echo "-- deposits --"
cleos push action eosio.token transfer  '["bob",  "vapaeetokens","10.0000 ACORN","deposit"]' -p bob@active
# cleos push action eosio.token transfer  '["alice","vapaeetokens","10.0000 ACORN","deposit"]' -p alice@active
# cleos push action eosio.token transfer  '["tom",  "vapaeetokens","10.0000 ACORN","deposit"]' -p tom@active
# cleos push action eosio.token transfer  '["kate", "vapaeetokens","10.0000 ACORN","deposit"]' -p kate@active

cleos push action vapaeetokens transfer '["bob",  "vapaeetokens","100.0000 CNT","deposit"]' -p bob@active
cleos push action vapaeetokens transfer '["alice","vapaeetokens","100.0000 CNT","deposit"]' -p alice@active
# cleos push action vapaeetokens transfer '["tom","vapaeetokens","0.1000 CNT","deposit"]' -p tom@active
# cleos push action vapaeetokens transfer '["kate","vapaeetokens","9.9999 CNT","deposit"]' -p kate@active

cleos push action eosio.token transfer  '["bob",  "vapaeetokens","100.0000 TLOS","deposit"]' -p bob@active
cleos push action eosio.token transfer  '["alice","vapaeetokens","100.0000 TLOS","deposit"]' -p alice@active

echo "-- 1 sell orders -> 1 buy order --"
sleep 2
cleos push action vapaeetokens order '["alice", "buy", "2.50000000 CNT", "0.40000000 TLOS"]' -p alice
cleos push action vapaeetokens order '["bob", "sell",  "2.50000000 CNT", "0.40000000 TLOS"]' -p bob
# 
echo "-- 3 buy orders --"
sleep 2
cleos push action vapaeetokens order '["alice", "buy", "5.00000000 CNT", "0.20000000 TLOS"]' -p alice
cleos push action vapaeetokens order '["alice", "buy","10.00000000 CNT", "0.10000000 TLOS"]' -p alice
cleos push action vapaeetokens order '["alice", "buy", "2.50000000 CNT", "0.40000000 TLOS"]' -p alice
#  
echo "-- 1 buy and 2 cancels --"
sleep 2
cleos push action vapaeetokens order '["bob",  "sell", "2.50000000 CNT", "0.40000000 TLOS"]' -p bob
cleos push action vapaeetokens cancel '["alice", "sell", "TLOS", "CNT", [1,0]]' -p alice
# 
echo "-- 3 orders --"
sleep 2
cleos push action vapaeetokens order '["alice", "buy", "5.00000000 CNT", "0.02000000 TLOS"]' -p alice
cleos push action vapaeetokens order '["alice", "buy","10.00000000 CNT", "0.01000000 TLOS"]' -p alice
cleos push action vapaeetokens order '["alice", "buy", "2.50000000 CNT", "0.04000000 TLOS"]' -p alice
# 
echo "-- 1 buy and overflows --"
sleep 2
cleos push action vapaeetokens order '["bob",  "sell", "5.00000000 CNT", "0.04000000 TLOS"]' -p bob
cleos push action vapaeetokens cancel '["bob", "sell", "CNT", "TLOS", [0]]' -p bob
cleos push action vapaeetokens cancel '["alice", "sell", "TLOS", "CNT", [0,1]]' -p alice

echo "-- 3 orders --"
sleep 2
cleos push action vapaeetokens order '["alice", "buy", "4.00000000 CNT", "0.20000000 TLOS"]' -p alice
cleos push action vapaeetokens order '["alice", "buy", "8.00000000 CNT", "0.10000000 TLOS"]' -p alice
cleos push action vapaeetokens order '["alice", "buy", "2.00000000 CNT", "0.40000000 TLOS"]' -p alice
### --------- vapaeetokens::sellorders(tlos.cnt) -------------------------------------------------
### {
###   "rows": [{
###       "id": 0,
###       "owner": "alice",
###       "price": "5.00000000 CNT",
###       "total": "4.00000000 CNT",
###       "selling": "0.80000000 TLOS",
###       "fee": "0.08000000 TLOS"
###     },{
###       "id": 1,
###       "owner": "alice",
###       "price": "10.00000000 CNT",
###       "total": "8.00000000 CNT",
###       "selling": "0.80000000 TLOS",
###       "fee": "0.08000000 TLOS"
###     },{
###       "id": 2,
###       "owner": "alice",
###       "price": "2.50000000 CNT",
###       "total": "2.00000000 CNT",
###       "selling": "0.80000000 TLOS",
###       "fee": "0.08000000 TLOS"
###     }
###   ],
###   "more": false
### }

# ### 
echo "-- 1 buy consumes 3 sell orders --"
cleos push action vapaeetokens order '["bob" , "sell", "7.00000000 CNT", "0.10000000 TLOS"]' -p bob

echo "-- 6 orders --"
cleos push action vapaeetokens order '["alice", "buy", "4.00000000 CNT", "0.05000000 TLOS"]' -p alice
cleos push action vapaeetokens order '["alice", "buy", "8.00000000 CNT", "0.06000000 TLOS"]' -p alice
cleos push action vapaeetokens order '["alice", "buy", "2.00000000 CNT", "0.07000000 TLOS"]' -p alice

cleos push action vapaeetokens order '["bob" , "sell", "7.00000000 CNT", "1.10000000 TLOS"]' -p bob
cleos push action vapaeetokens order '["bob" , "sell", "2.00000000 CNT", "1.20000000 TLOS"]' -p bob
cleos push action vapaeetokens order '["bob" , "sell", "3.00000000 CNT", "1.30000000 TLOS"]' -p bob

# cleos push action vapaeetokens withdraw '["alice", "9.0000 ACORN"]' -p alice
# cleos push action vapaeetokens withdraw '["bob", "4.0000 ACORN"]' -p bob

# cleos push action eosio.token transfer '["alice","vapaeetokens","8.0000 ACORN","deposit"]' -p alice@active
# cleos push action eosio.token transfer '["bob","vapaeetokens","3.0000 ACORN","deposit"]' -p bob@active