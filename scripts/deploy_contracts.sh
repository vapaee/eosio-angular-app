#!/bin/bash

# checking 'force' param
force=false
NET=
if [ "$1" == "force" ]; then
   force=true
fi

if [ "$2" == "force" ]; then
   force=true
fi

if [ "$1" == "test" ]; then
   NET='--url https://testnet.telos.caleos.io'
fi

if [ "$2" == "test" ]; then
   NET='--url https://testnet.telos.caleos.io'
fi

if [ "$1" == "prod" ]; then
   NET='--url https://telos.eos.barcelona'
fi

if [ "$2" == "prod" ]; then
   NET='--url https://telos.eos.barcelona'
fi

HOME=/var/www/eosio-angular-app
HELLO_HOME=$HOME/contracts/_examples/hello
ADDRESSBOOK_HOME=$HOME/contracts/_examples/addressbook
ABCOUNTER_HOME=$HOME/contracts/_examples/abcounter
YOUVOTE_HOME=$HOME/contracts/_examples/youvote
APPSERVER_HOME=$HOME/contracts/_examples/appserver
APPPLUGIN_HOME=$HOME/contracts/_examples/appplugin

CARDSNTOKENS_HOME=$HOME/contracts/cardsntokens
VAPAEETOKENS_HOME=$HOME/airdrop-on-telos/contracts/vapaeetokens
SNAPSNAPSNAP_HOME=$HOME/airdrop-on-telos/contracts/snapsnapsnap
BOARDGAMEBOX_HOME=$HOME/contracts/boardgamebox
LOCALSTRINGS_HOME=$HOME/contracts/localstrings

TESTING_HOME=$HOME/contracts/testing

# echo "-------- appserver ---------"
# cd $APPSERVER_HOME
# if [[ appserver.cpp -nt appserver.wasm ]]; then
#     eosio-cpp -o appserver.wasm appserver.cpp --abigen
#     cleos set contract appserver $PWD -p appserver@active
# fi
# 
# echo "-------- appplugin ---------"
# cd $APPPLUGIN_HOME
# if [[ appplugin.cpp -nt appplugin.wasm ]]; then
#     eosio-cpp -o appplugin.wasm appplugin.cpp --abigen
#     cleos set contract appplugin $PWD -p appplugin@active
# fi


# echo "-------- hello ---------"
# cd $HELLO_HOME
# if [[ hello.cpp -nt hello.wasm ]]; then
#     eosio-cpp -o hello.wasm hello.cpp --abigen
#     cleos set contract hello $PWD -p hello@active
# fi
# 
# echo "-------- addressbook ---------"
# cd $ADDRESSBOOK_HOME
# if [[ addressbook.cpp -nt addressbook.wasm ]]; then
#     eosio-cpp -o addressbook.wasm addressbook.cpp --abigen
#     cleos set contract addressbook $PWD -p addressbook@active
# fi
# 
# echo "-------- abcounter ---------"
# cd $ABCOUNTER_HOME
# if [[ abcounter.cpp -nt abcounter.wasm ]]; then
#     eosio-cpp -o abcounter.wasm abcounter.cpp --abigen
#     cleos set contract abcounter $PWD -p abcounter@active
# fi
# 
# echo "-------- youvote ---------"
# cd $YOUVOTE_HOME
# if [[ youvote.cpp -nt youvote.wasm ]]; then
#     eosio-cpp -o youvote.wasm youvote.cpp --abigen
#     cleos set contract youvote $PWD -p youvote@active
# fi

# --------------------------------------------------------------------

echo "-------- boardgamebox ---------"
cd $BOARDGAMEBOX_HOME
if [[ boardgamebox.core.hpp -nt boardgamebox.wasm || 
      boardgamebox.author.hpp -nt boardgamebox.wasm ||
      boardgamebox.utils.hpp -nt boardgamebox.wasm ||
      boardgamebox.cpp -nt boardgamebox.wasm ||
      boardgamebox.hpp -nt boardgamebox.wasm || 
      $force == true ]]; then
    eosio-cpp -o boardgamebox.wasm boardgamebox.cpp --abigen -I ../includes
    cleos $NET set contract boardgamebox $PWD -p boardgamebox@active
fi

echo "-------- cardsntokens ---------"
cd $CARDSNTOKENS_HOME
if [[ cardsntokens.cpp -nt cardsntokens.wasm || cardsntokens.hpp -nt cardsntokens.wasm || $force == true ]]; then
    # echo "skipping..."
    eosio-cpp -o cardsntokens.wasm cardsntokens.cpp --abigen -I ../includes
    cleos $NET set contract cardsntokens $PWD -p cardsntokens@active
fi

echo "-------- localstrings ---------"
cd $LOCALSTRINGS_HOME
if [[ localstrings.cpp -nt localstrings.wasm || localstrings.hpp -nt localstrings.wasm || $force == true ]]; then
    # echo "skipping..."
    eosio-cpp -o localstrings.wasm localstrings.cpp --abigen -I ../includes
    cleos $NET set contract localstrings $PWD -p localstrings@active
fi

echo "-------- vapaeetokens ---------"
cd $VAPAEETOKENS_HOME
if [[ vapaeetokens.cpp -nt vapaeetokens.wasm || vapaeetokens.hpp -nt vapaeetokens.wasm || $force == true ]]; then
    eosio-cpp -o vapaeetokens.wasm vapaeetokens.cpp --abigen
    cleos $NET set contract vapaeetokens $PWD -p vapaeetokens@active
fi

echo "-------- snapsnapsnap ---------"
cd $SNAPSNAPSNAP_HOME
if [[ snapsnapsnap.cpp -nt snapsnapsnap.wasm || snapsnapsnap.hpp -nt snapsnapsnap.wasm || $force == true ]]; then
    eosio-cpp -o snapsnapsnap.wasm snapsnapsnap.cpp --abigen
    cleos $NET set contract snapsnapsnap $PWD -p snapsnapsnap@active
fi