#!/bin/bash
HOME=/var/www/eosio-angular-app
HELLO_HOME=$HOME/contracts/_examples/hello
ADDRESSBOOK_HOME=$HOME/contracts/_examples/addressbook
ABCOUNTER_HOME=$HOME/contracts/_examples/abcounter
YOUVOTE_HOME=$HOME/contracts/_examples/youvote

CARDSNTOKENS_HOME=$HOME/contracts/cardsntokens
VAPAEETOKENS_HOME=$HOME/contracts/vapaeetokens
BOARDGAMEBOX_HOME=$HOME/contracts/boardbagebox
VAPAEEAUTHOR_HOME=$HOME/contracts/vapaeeauthor

TESTING_HOME=$HOME/contracts/testing

echo "-------- hello ---------"
cd $HELLO_HOME
if [[ hello.cpp -nt hello.wasm ]]; then
    eosio-cpp -o hello.wasm hello.cpp --abigen
    cleos set contract hello $PWD -p hello@active
fi

echo "-------- addressbook ---------"
cd $ADDRESSBOOK_HOME
if [[ addressbook.cpp -nt addressbook.wasm ]]; then
    eosio-cpp -o addressbook.wasm addressbook.cpp --abigen
    cleos set contract addressbook $PWD -p addressbook@active
fi

echo "-------- abcounter ---------"
cd $ABCOUNTER_HOME
if [[ abcounter.cpp -nt abcounter.wasm ]]; then
    eosio-cpp -o abcounter.wasm abcounter.cpp --abigen
    cleos set contract abcounter $PWD -p abcounter@active
fi

# echo "-------- youvote ---------"
# cd $YOUVOTE_HOME
# if [[ youvote.cpp -nt youvote.wasm ]]; then
#     eosio-cpp -o youvote.wasm youvote.cpp --abigen
#     cleos set contract youvote $PWD -p youvote@active
# fi

# --------------------------------------------------------------------

echo "-------- boardbagebox ---------"
cd $BOARDGAMEBOX_HOME
if [[ boardbagebox.cpp -nt boardbagebox.wasm || boardbagebox.hpp -nt boardbagebox.wasm ]]; then
    eosio-cpp -o boardbagebox.wasm boardbagebox.cpp --abigen
    cleos set contract boardbagebox $PWD -p boardbagebox@active
fi

echo "-------- vapaeeauthor ---------"
cd $VAPAEEAUTHOR_HOME
if [[ vapaeeauthor.cpp -nt vapaeeauthor.wasm || vapaeeauthor.hpp -nt vapaeeauthor.wasm ]]; then
    eosio-cpp -o vapaeeauthor.wasm vapaeeauthor.cpp --abigen
    cleos set contract vapaeeauthor $PWD -p vapaeeauthor@active
fi

echo "-------- vapaeetokens ---------"
cd $VAPAEETOKENS_HOME
if [[ vapaeetokens.cpp -nt vapaeetokens.wasm || vapaeetokens.hpp -nt vapaeetokens.wasm ]]; then
    eosio-cpp -o vapaeetokens.wasm vapaeetokens.cpp --abigen
    cleos set contract vapaeetokens $PWD -p vapaeetokens@active
fi

echo "-------- cardsntokens ---------"
cd $CARDSNTOKENS_HOME
if [[ cardsntokens.cpp -nt cardsntokens.wasm || cardsntokens.hpp -nt cardsntokens.wasm ]]; then
    # echo "skipping..."
    eosio-cpp -o cardsntokens.wasm cardsntokens.cpp --abigen -I ../includes
    cleos set contract cardsntokens $PWD -p cardsntokens@active
fi


#echo "-------- test ---------"
# cd $TESTING_HOME
# echo $PWD
# echo -e "\n\n"
# g++ -o main -I ../includes main.cpp -I /usr/opt/eosio.cdt/1.4.1/include -I /usr/opt/eosio.cdt/1.4.1/include/libcxx && ./main    
