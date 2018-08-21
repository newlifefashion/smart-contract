#!/usr/bin/env bash
set -x
set -e

cleos ${remote} push action eosio.token transfer '[bob111111111, '"${contract}"', "10.0000 EOS","by transfer"]' -p bob111111111
#cleos ${remote} push action ${contract} transfer '[alice1111111, '"${contract}"', "10.0000 EOS","by transfer"]' -p alice1111111
