#!/bin/bash
HOME=/var/www/eosio-angular-app
HELLO_HOME=$HOME/contracts/_examples/hello
ADDRESSBOOK_HOME=$HOME/contracts/_examples/addressbook
ABCOUNTER_HOME=$HOME/contracts/_examples/abcounter

CARDSNTOKENS_HOME=$HOME/contracts/cardsntokens
VAPAEETOKENS_HOME=$HOME/contracts/vapaeetokens
BOARDGAMEBOX_HOME=$HOME/contracts/boardbagebox

echo "-------- hello ---------"
cd $HELLO_HOME
if [[ hello.cpp -nt hello.wasm ]]; then
    eosio-cpp -o hello.wasm hello.cpp --abigen
    cleos set contract hello $PWD
fi

echo "-------- addressbook ---------"
cd $ADDRESSBOOK_HOME
if [[ addressbook.cpp -nt addressbook.wasm ]]; then
    eosio-cpp -o addressbook.wasm addressbook.cpp --abigen
    cleos set contract addressbook $PWD
fi

echo "-------- abcounter ---------"
cd $ABCOUNTER_HOME
if [[ abcounter.cpp -nt abcounter.wasm ]]; then
    eosio-cpp -o abcounter.wasm abcounter.cpp --abigen
    cleos set contract abcounter $PWD
fi

# --------------------------------------------------------------------

echo "-------- boardbagebox ---------"
cd $BOARDGAMEBOX_HOME
if [[ boardbagebox.cpp -nt boardbagebox.wasm ]]; then
    eosio-cpp -o boardbagebox.wasm boardbagebox.cpp --abigen
    cleos set contract boardbagebox $PWD
fi

echo "-------- vapaeetokens ---------"
cd $VAPAEETOKENS_HOME
if [[ vapaeetokens.cpp -nt vapaeetokens.wasm ]]; then
    eosio-cpp -o vapaeetokens.wasm vapaeetokens.cpp --abigen
    cleos set contract vapaeetokens $PWD
fi

echo "-------- cardsntokens ---------"
cd $CARDSNTOKENS_HOME
if [[ cardsntokens.cpp -nt cardsntokens.wasm ]]; then
    eosio-cpp -o cardsntokens.wasm cardsntokens.cpp --abigen
    cleos set contract cardsntokens $PWD
fi

