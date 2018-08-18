#!/usr/bin/env bash
set -x
set -e

#cleos ${remote} push action ${contract} issue '[alice1111111, "1000 EOS","issue"]' -p alice1111111
cleos ${remote} get table ${contract} alice1111111 accounts
cleos ${remote} get table ${contract} bob111111111 accounts