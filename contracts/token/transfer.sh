#!/usr/bin/env bash
set -x
set -e

cleos ${remote} push action ${contract} transfer '[alice1111111, bob111111111, "10 EOS","by transfer"]' -p alice1111111