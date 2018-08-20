#!/usr/bin/env bash
set -x
set -e

cleos ${remote} push action ${contract} grantvesting '[alice1111111, bob111111111, "100.0000 NEW", "2018-08-19T23:45:41"]' -p alice1111111