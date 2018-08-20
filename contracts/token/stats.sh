#!/usr/bin/env bash
set -x
set -e

cleos ${remote} get table ${contract} EOS stat
cleos ${remote} get table ${contract} AAA stat