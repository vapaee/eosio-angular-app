#!/bin/bash

if [ "$1" == "" ]; then
   echo "ERROR: user name needed"
   exit;
fi
if [ "$2" == "" ]; then
   echo "ERROR: token symbol needed"
   exit;
fi
user=$1
SYM=$2

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

echo "********* vapaeetokens $SYM token for $user *******************************************************"
show_balance vapaeetokens $user $SYM

show_table vapaeetokens $user stakes
show_table vapaeetokens $user unstakes
show_table vapaeetokens $user config
