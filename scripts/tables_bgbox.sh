#!/bin/bash

show_table() {
    scope=$1
    name=$2

    echo "--------- $name ($scope) -------------------------------------------------"
    cleos get table boardgamebox $scope $name
}

show_table boardgamebox authors
show_table boardgamebox apps
show_table boardgamebox profiles

show_table boardgamebox contspec
show_table boardgamebox contasset

show_table 1 containers
show_table 2 containers
