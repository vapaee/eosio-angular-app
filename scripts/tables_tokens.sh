#!/bin/bash

show_table() {
    code=$1
    scope=$2
    table=$3

    echo "--------- $code::$table($scope) -------------------------------------------------"
    cleos get table $code $scope $table
}

show_balance() {
    code=$1
    scope=$2
    symbol=$3
    echo "--------- $scope ($symbol) -------------------------------------------------"
    cleos get currency balance $code $scope $symbol
}

show_table snapsnapsnap 1 snapshots
show_table vapaeetokens bob accounts
show_table vapaeetokens alice accounts
show_table vapaeetokens viter accounts

# show_balance eosio.token bob SYS
# show_balance eosio.token alice SYS

show_balance vapaeetokens bob CNT
show_balance vapaeetokens alice CNT
show_balance vapaeetokens tom CNT
show_balance vapaeetokens kate CNT
show_balance vapaeetokens viter CNT

echo "--------- vapaeetokens CNT-------------------------------------------------"
cleos get currency stats vapaeetokens CNT
show_table vapaeetokens CNT source