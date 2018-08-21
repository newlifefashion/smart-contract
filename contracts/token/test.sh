#!/usr/bin/env bash
set -x
set -e

. ./env.sh $1

./new.sh
./build.sh
./deploy.sh
./create.sh
./issue.sh
./accounts.sh
./stats.sh
./grantvesting.sh
./vestingassets.sh
./unlock.sh
./accounts.sh