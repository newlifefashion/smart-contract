#!/usr/bin/env bash
set -x
set -e

cleos ${remote} push action ${contract} issue '[bob111111111, "1000.0000 NEW","issue"]' -p alice1111111