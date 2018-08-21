#!/usr/bin/env bash
set -x
set -e

cleos ${remote} push action ${contract} transfer '[bob111111111, alice1111111, "10.0000 NEW","by transfer"]' -p bob111111111
cleos ${remote} push action ${contract} transfer '[alice1111111, bob111111111, "10.0000 NEW","by transfer"]' -p alice1111111
