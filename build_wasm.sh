#!/bin/zsh

set +e

mkdir -p wasm
emcc -Oz -o wasm/lifehash.mjs src/*.cpp
