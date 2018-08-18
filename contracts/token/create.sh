#!/usr/bin/env bash
set -x
set -e

cleos ${remote} push action ${contract} create '[alice1111111, "1000 EOS"]' -p ${contract}