#!/bin/bash

echo "----- loading tokens ----"
echo "-- snapshot --"
cleos push action snapsnapsnap add '["bob",250000000,1]' -p bob@active
cleos push action snapsnapsnap add '["alice",10000000000,1]' -p alice@active
cleos push action snapsnapsnap add '["tom",1000,1]' -p tom@active
cleos push action snapsnapsnap add '["kate",9999999,1]' -p kate@active


# register TLOS & ACORN token
echo "-- register TLOS --" 
cleos push action vapaeetokens addtoken '["eosio.token","TLOS",4,"vapaeetokens"]' -p vapaeetokens@active
cleos push action vapaeetokens addtoken '["eosio.token","TLOS",4,"vapaeetokens"]' -p vapaeetokens@active
cleos push action vapaeetokens updatetoken '["TLOS", "Telos", "https://telosfoundation.io", "/assets/logos/telos.png", "/assets/logos/telos-lg.png",true]' -p vapaeetokens@active
echo "-- register VIITA --" 
cleos push action vapaeetokens addtoken '["eosio.token","VIITA",4,"vapaeetokens"]' -p vapaeetokens@active
cleos push action vapaeetokens updatetoken '["VIITA", "Viitasphere", "https://viitasphere.com", "/assets/logos/viitasphere.png", "/assets/logos/viitasphere-lg.png",true]' -p vapaeetokens@active
echo "-- register VIICT --" 
cleos push action vapaeetokens addtoken '["eosio.token","VIICT",0,"vapaeetokens"]' -p vapaeetokens@active
cleos push action vapaeetokens updatetoken '["VIICT", "Viitasphere", "https://viitasphere.com", "/assets/logos/viitasphere.png", "/assets/logos/viitasphere-lg.png",true]' -p vapaeetokens@active
echo "-- register QBE --" 
cleos push action vapaeetokens addtoken '["eosio.token","QBE",4,"vapaeetokens"]' -p vapaeetokens@active
cleos push action vapaeetokens updatetoken '["QBE", "Qubicles", "https://fenero.io/qubicles", "/assets/logos/qbe.png", "/assets/logos/qbe-lg.png",true]' -p vapaeetokens@active
echo "-- register ACORN --" 
cleos push action vapaeetokens addtoken '["eosio.token","ACORN",4,"vapaeetokens"]' -p vapaeetokens@active
cleos push action vapaeetokens updatetoken '["ACORN", "ACORN", "http://acorns.fun", "/assets/logos/acorn.svg", "/assets/logos/acorn-lg.png",true]' -p vapaeetokens@active
echo "-- register EDNA --" 
cleos push action vapaeetokens addtoken '["eosio.token","EDNA",4,"vapaeetokens"]' -p vapaeetokens@active
cleos push action vapaeetokens updatetoken '["EDNA", "EDNA", "https://github.com/EDNA-LIFE", "/assets/logos/edna.png", "/assets/logos/edna-lg.png",true]' -p vapaeetokens@active
echo "-- register TEACH --" 
cleos push action vapaeetokens addtoken '["eosio.token","TEACH",4,"vapaeetokens"]' -p vapaeetokens@active
cleos push action vapaeetokens updatetoken '["TEACH", "Teachology", "http://teachology.io", "/assets/logos/teach.svg", "/assets/logos/teach-lg.png",true]' -p vapaeetokens@active
echo "-- register ROBO --" 
cleos push action vapaeetokens addtoken '["eosio.token","ROBO",4,"vapaeetokens"]' -p vapaeetokens@active
cleos push action vapaeetokens updatetoken '["ROBO", "Proxibots", "https://proxibots.io", "/assets/logos/proxibots.png", "/assets/logos/proxibots-lg.png",true]' -p vapaeetokens@active


echo "-- creating CNT token --" 
cleos push action vapaeetokens create '["vapaeetokens","500000000.0000 CNT"]' -p vapaeetokens@active
cleos push action vapaeetokens updatetoken '["CNT", "Cards & Tokens", "http://cardsandtokens.com", "/assets/logos/cnt.svg", "/assets/logos/cnt-lg.svg",true]' -p vapaeetokens@active
echo "-- creating BOX token --" 
cleos push action vapaeetokens create '["vapaeetokens","500000000.0000 BOX"]' -p vapaeetokens@active
cleos push action vapaeetokens updatetoken '["BOX", "Board Game Box", "http://bgbox.com", "/assets/logos/box.png", "/assets/logos/box-lg.png",true]' -p vapaeetokens@active
echo "-- creating VPE token --" 
cleos push action vapaeetokens create '["vapaeetokens","1000000.000000 VPE"]' -p vapaeetokens@active
cleos push action vapaeetokens updatetoken '["VPE", "Vapaée", "https://vapaee.io", "/assets/logos/vapaee.png", "/assets/logos/vapaee-lg.png",true]' -p vapaeetokens@active

# ratio 1:1
echo "-- set setsnapshot for CNT --" 
cleos push action vapaeetokens setsnapshot '["snapsnapsnap",1,"CNT",0,0,10000,0,"CNT Token Airdrop - Cards & Tokens - http://cardsandtokens.com"]' -p vapaeetokens@active

# ratio 1:1 - min 1000.0000 TLOS - cap 40000.0000 TLOS
echo "-- set setsnapshot for BOX --" 
cleos push action vapaeetokens setsnapshot '["snapsnapsnap",1,"BOX",400000000,10000000,1000,0,"BOX Token Airdrop - Board Game Box"]' -p vapaeetokens@active

# ratio 1:0 - min 1000.0000 TLOS - base 1.0000 VPE
echo "-- set setsnapshot for VPE --" 
cleos push action vapaeetokens setsnapshot '["snapsnapsnap",1,"VPE",0,10000000,0,10000,"VPE Token Airdrop - Vapaee"]' -p vapaeetokens@active


# add issuers to the tokens
echo "-- adding issuers to the tokens --" 
cleos push action vapaeetokens addissuer '["boardgamebox","BOX"]' -p vapaeetokens@active
cleos push action vapaeetokens addissuer '["cardsntokens","CNT"]' -p vapaeetokens@active
cleos push action vapaeetokens addissuer '["vapaeetokens","VPE"]' -p vapaeetokens@active


echo "-------- eosio.token (HEART) ---------"
cleos push action revelation21 create '["revelation21", "2100000000.0000 HEART"]' -p revelation21@active
cleos get currency stats revelation21 HEART
cleos push action revelation21 open '["alice", "4,HEART", "alice"]' -p alice@active
cleos push action revelation21 open '["bob", "4,HEART", "bob"]' -p bob@active
cleos push action revelation21 open '["tom", "4,HEART", "tom"]' -p tom@active
cleos push action revelation21 open '["user1.jc", "4,HEART", "user1.jc"]' -p user1.jc@active
cleos push action revelation21 open '["user2.jc", "4,HEART", "user2.jc"]' -p user2.jc@active

cleos get currency balance revelation21 alice HEART
cleos get currency balance revelation21 bob HEART
cleos get currency balance revelation21 tom HEART
cleos get currency balance revelation21 user1.jc HEART
cleos get currency balance revelation21 user2.jc HEART
cleos get currency balance revelation21 user3.jc HEART


cleos push action revelation21 transfer '["user2.jc","user3.jc","1.0000 HEART",""]' -p user2.jc@active
cleos get currency balance revelation21 user2.jc HEART
cleos get currency balance revelation21 user3.jc HEART

cleos push action revelation21 transfer '["user3.jc","kate","0.1000 HEART",""]' -p user3.jc@active
cleos get currency balance revelation21 kate HEART
cleos get currency balance revelation21 user3.jc HEART
cleos push action revelation21 transfer '["kate","user3.jc","0.1000 HEART",""]' -p kate@active
cleos get currency balance revelation21 kate HEART
cleos get currency balance revelation21 user3.jc HEART
