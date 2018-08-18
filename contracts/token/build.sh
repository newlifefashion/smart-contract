#!/usr/bin/env bash
set -x
set -e

eosiocpp -g ${file}.abi ${file}.hpp
eosiocpp -o ${file}.wasm ${file}.cpp