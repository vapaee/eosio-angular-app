#!/bin/bash

show_table() {
    code=$1
    scope=$2
    table=$3

    echo "--------- $code::$table ($scope) -------------------------------------------------"
    cleos get table $code $scope $table
}

show_table boardgamebox boardgamebox authors
show_table boardgamebox boardgamebox apps
show_table boardgamebox boardgamebox profiles

show_table boardgamebox boardgamebox contspec
show_table boardgamebox boardgamebox contasset

show_table boardgamebox 1 containers
show_table boardgamebox 2 containers