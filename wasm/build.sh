#!/bin/zsh

set +e

mkdir -p lib

emcc \
    -o ./lib/lifehash.js \
    -Oz \
    -s EXPORTED_FUNCTIONS='[_free,_malloc]' \
    -s EXPORT_ES6=1 \
    -s MODULARIZE=1 \
    -s ENVIRONMENT='web' \
    -s NO_DISABLE_EXCEPTION_CATCHING \
    --post-js lifehash.post.js \
    ../src/*.cpp

cp lifehash.types.ts lifehash.d.ts ./lib/
