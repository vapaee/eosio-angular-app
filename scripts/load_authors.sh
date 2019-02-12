#!/bin/bash



echo "------------------- load authors ----------------------"
cleos push action boardgamebox newapp '["boardgamebox", "boardgamebox", "board-game-box", "Board Game Box", 0]' -p boardgamebox@active
cleos push action boardgamebox newapp '["boardgamebox", "boardgamebox", "board-game-box", "Board Game Box", 0]' -p boardgamebox@active

cleos push action boardgamebox newapp '["cardsntokens", "cardsntokens", "cards-and-tokens", "Cards & Tokens", 8]' -p cardsntokens@active
cleos push action boardgamebox newapp '["vapaeetokens", "vapaeetokens", "vapaee", "Vapaée", 0]' -p vapaeetokens@active

cleos push action boardgamebox newprofile '["bob", "bob-constructor", "BoB Constructor"]' -p bob@active
cleos push action boardgamebox newprofile '["alice", "wonder-alice", "Alice"]' -p alice@active