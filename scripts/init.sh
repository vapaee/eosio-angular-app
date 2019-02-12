#!/bin/bash
# rm /var/www/blockchain/eosio/data -fr

./create_accounts.sh

./deploy_contracts.sh

./load_authors.sh

./load_tokens.sh

