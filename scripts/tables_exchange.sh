#!/bin/bash

show_balance() {
    user=$1
    echo "--------- balances for $user -------------------------------------------------"
    cleos get currency balance vapaeetokens $user CNT
    cleos get currency balance eosio.token $user TLOS
    cleos get currency balance eosio.token $user ACORN
    echo " -- deposits --"
    cleos get table vapaeetokens $user deposits
}

show_table() {
    code=$1
    scope=$2
    table=$3

    echo "--------- $code::$table($scope) -------------------------------------------------"
    cleos get table $code $scope $table
}

show_table vapaeetokens vapaeetokens feeconfig

show_table vapaeetokens vapaeetokens tokens

show_table vapaeetokens tlos.cnt sellorders
show_table vapaeetokens cnt.tlos sellorders

show_balance bob
show_balance alice
#show_balance tom
#show_balance kate
show_balance vapaeetokens

show_table vapaeetokens vapaeetokens ordertables
show_table vapaeetokens vapaeetokens earnings
show_table vapaeetokens cnt.tlos history