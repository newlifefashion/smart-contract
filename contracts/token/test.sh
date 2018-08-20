#!/usr/bin/env bash
set -x
set -e

./build.sh
./deploy.sh
./create.sh
./issue.sh
./accounts.sh
./stats.sh