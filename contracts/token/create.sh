#!/usr/bin/env bash
set -x
set -e

cleos ${remote} push action ${contract} create '[alice1111111, "1000.0000 NEW"]' -p ${contract}
