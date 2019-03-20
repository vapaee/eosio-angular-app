#!/bin/bash



echo "------------------- load authors ----------------------"
echo "-- boardgamebox --"
cleos push action boardgamebox newapp '["boardgamebox", "boardgamebox", "board-game-box", "Board Game Box", 0]' -p boardgamebox@active
echo "-- cardsntokens --"
cleos push action boardgamebox newapp '["cardsntokens", "cardsntokens", "cards-and-tokens", "Cards & Tokens", 8]' -p cardsntokens@active
echo "-- vapaeetokens --"
cleos push action boardgamebox newapp '["vapaeetokens", "vapaeetokens", "vapaee", "Vapaée", 0]' -p vapaeetokens@active

echo "-- 2 users --"
cleos push action boardgamebox newprofile '["bob", "bob-constructor", "BoB Constructor"]' -p bob@active
cleos push action boardgamebox newprofile '["alice", "wonder-alice", "Alice"]' -p alice@active
