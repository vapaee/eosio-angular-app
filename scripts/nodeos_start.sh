#!/bin/bash
mkdir -p /var/www/blockchain/eosio
echo "-------------------------------------------------------------------------" >> /var/www/blockchain/eosio/nodeos.log

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
echo "tail -f /var/www/blockchain/eosio/nodeos.log"

