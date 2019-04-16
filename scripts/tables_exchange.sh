#!/bin/bash
show_balance() {
    user=$1
    echo "********* balances for $user -------------------------------------------------"
    cleos get currency balance vapaeetokens $user CNT
    cleos get currency balance eosio.token $user TLOS
    cleos get currency balance eosio.token $user ACORN
    cleos get currency balance vapaeetokens $user BOX
    echo " -- deposits --"
    cleos get table vapaeetokens $user deposits
    echo " -- userorders --"
    cleos get table vapaeetokens $user userorders
}

show_table() {
    code=$1
    scope=$2
    table=$3

    echo "--------- $code::$table($scope) -------------------------------------------------"
    cleos get table $code $scope $table -l 50
}

show_table vapaeetokens vapaeetokens feeconfig

show_table vapaeetokens vapaeetokens tokens

show_table vapaeetokens tlos.cnt sellorders
show_table vapaeetokens cnt.tlos sellorders
# show_table vapaeetokens tlos.box sellorders
# show_table vapaeetokens box.tlos sellorders

show_balance bob
show_balance alice
show_balance tom
show_balance kate
show_balance vapaeetokens

show_table vapaeetokens vapaeetokens depusers
show_table vapaeetokens vapaeetokens ordertables
show_table vapaeetokens vapaeetokens earnings
show_table vapaeetokens cnt.tlos tablesummary
show_table vapaeetokens cnt.tlos history
# show_table vapaeetokens box.tlos tablesummary
# show_table vapaeetokens box.tlos history

#cleos push action vapaeetokens poblate '["alice", "acorn.tlos"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["alice", "box.tlos"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["alice", "cnt.tlos"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["alice", "edna.tlos"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["alice", "robo.tlos"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["alice", "tlos.acorn"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["alice", "tlos.cnt"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["alice", "tlos.box"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["alice", "tlos.robo"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["alice", "tlos.teach"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["alice", "tlos.vpe"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["bob", "acorn.tlos"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["bob", "box.tlos"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["bob", "cnt.tlos"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["bob", "edna.tlos"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["bob", "robo.tlos"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["bob", "tlos.acorn"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["bob", "tlos.cnt"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["bob", "tlos.box"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["bob", "tlos.robo"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["bob", "tlos.teach"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["bob", "tlos.vpe"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["kate", "acorn.tlos"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["kate", "box.tlos"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["kate", "cnt.tlos"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["kate", "edna.tlos"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["kate", "robo.tlos"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["kate", "tlos.acorn"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["kate", "tlos.cnt"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["kate", "tlos.box"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["kate", "tlos.robo"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["kate", "tlos.teach"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["kate", "tlos.vpe"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["tom", "acorn.tlos"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["tom", "box.tlos"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["tom", "cnt.tlos"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["tom", "edna.tlos"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["tom", "robo.tlos"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["tom", "tlos.acorn"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["tom", "tlos.cnt"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["tom", "tlos.box"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["tom", "tlos.robo"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["tom", "tlos.teach"]' -p vapaeetokens
#cleos push action vapaeetokens poblate '["tom", "tlos.vpe"]' -p vapaeetokens
