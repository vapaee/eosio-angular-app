#!/bin/bash

show_table() {
    code=$1
    scope=$2
    table=$3

    echo "--------- $code::$table($scope) -------------------------------------------------"
    cleos get table $code $scope $table
}

show_table vapaeetokens vapaeetokens tokens

show_table vapaeetokens vpe.tlos buyorders
show_table vapaeetokens vpe.tlos sellorders

