#!/usr/bin/env bash
set -e
set -x

#./build.sh

cd ../../build/contracts/hello
cleos ${remote} set contract ${contract} . hello.wasm hello.abi -p ${contract}
cd -

cleos ${remote} set contract ${contract} . ${file}.wasm ${file}.abi -p ${contract}