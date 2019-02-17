#!/bin/bash

show_balance() {
    user=$1
    echo "--------- balances for $user -------------------------------------------------"
    cleos get currency balance vapaeetokens $user CNT
    cleos get currency balance eosio.token $user TLOS

}

show_table() {
    code=$1
    scope=$2
    table=$3

    echo "--------- $code::$table($scope) -------------------------------------------------"
    cleos get table $code $scope $table
}

show_table vapaeetokens vapaeetokens tokens

show_table vapaeetokens cnt.tlos sellorders
show_table vapaeetokens cnt.tlos buyorders

show_balance bob
show_balance alice
# show_balance tom
# show_balance kate