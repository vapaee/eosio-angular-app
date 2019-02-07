#!/bin/bash


cleos push action snapsnapsnap add '["bob",250000000,1]' -p bob@active
cleos push action snapsnapsnap add '["alice",1110000,1]' -p alice@active
cleos push action snapsnapsnap add '["tom",1000,1]' -p tom@active
cleos push action snapsnapsnap add '["kate",9999,1]' -p kate@active
cleos push action snapsnapsnap add '["viter",210000000,1]' -p viter@active

cleos push action vapaeetokens create '["vapaeetokens","500000000.0000 CNT"]' -p vapaeetokens@active
cleos push action vapaeetokens setsnapshot '["snapsnapsnap",1,"CNT",0,0]' -p vapaeetokens@active

# en los siguientes ejemplos bob paga la ram de todos
# cleos push action vapaeetokens claim '["bob","CNT","bob"]' -p bob@active
# cleos push action vapaeetokens claim '["alice","CNT","bob"]' -p bob@active
# cleos push action vapaeetokens claim '["kate","CNT","bob"]' -p bob@active

