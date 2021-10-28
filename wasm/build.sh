#!/bin/zsh

set +e

emcc -Oz -o lifehash.js ../src/*.cpp

mkdir -p lib

emcc \
    ../src/*.cpp \
    -Oz \
    -o ./lib/lifehash.js \
    -s EXPORTED_FUNCTIONS='[_free,_malloc]' \
    -s EXPORT_ES6=1 \
    -s MODULARIZE=1 \
    -s ENVIRONMENT='web' \
    -s NO_DISABLE_EXCEPTION_CATCHING \
    --post-js lifehash.post.js

cp lifehash.d.ts ./lib/
