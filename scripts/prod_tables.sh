#!/bin/bash

NET='--url https://telos.eos.barcelona'

show_table() {
    code=$1
    scope=$2
    table=$3

    echo "--------- $code::$table($scope) -------------------------------------------------"
    cleos $NET get table $code $scope $table -l 50
}


show_table vapaeetokens aaa.tlos history
show_table vapaeetokens bbb.tlos history
show_table vapaeetokens ccc.tlos history
show_table vapaeetokens ddd.tlos history
show_table vapaeetokens eee.tlos history
show_table vapaeetokens fff.tlos history
