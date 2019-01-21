#!/bin/bash
mkdir -p /var/www/blockchain/eosio
echo "-------------------------------------------------------------------------" >> /var/www/blockchain/eosio/nodeos.log

#making sure koesd is running will 'infinity' autolock timeout;
cleos wallet stop
keosd --unlock-timeout=9999999 &
sleep 1
echo "PW5KG6YzAowdzRaNWEH1PterYoYUCPpBpMYxNN3EctcECw1Ke5vgS (home)"
echo "PW5KVpRdxvAWfAYVD62sNWqyVN7R4fohFQC7VVgYpXKNrbRCTYRBv (im)"
cleos wallet unlock

sleep 2

nodeos -e -p eosio \
--plugin eosio::producer_plugin \
--plugin eosio::chain_api_plugin \
--plugin eosio::http_plugin \
--plugin eosio::history_api_plugin \
-d /var/www/blockchain/eosio/data \
--config-dir /var/www/blockchain/eosio/config \
--access-control-allow-origin=* \
--contracts-console \
--http-validate-host=false \
—filter-on=‘*’ >> /var/www/blockchain/eosio/nodeos.log 2>&1 &


tail -f /var/www/blockchain/eosio/nodeos.log

