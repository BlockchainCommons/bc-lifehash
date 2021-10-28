#!/bin/zsh

set +e

emcc -Oz -o lifehash.js ../src/*.cpp

mkdir -p lib

exported_functions=[\
_free,\
_malloc,\
_lifehash_image_free,\
_lifehash_make_from_utf8,\
_lifehash_make_from_data,\
_lifehash_make_from_digest\
]

emcc \
    ../src/*.cpp \
    -Oz \
    -o ./lib/lifehash.js \
    -s EXPORTED_FUNCTIONS=${exported_functions} \
    -s EXPORT_ES6=1 \
    -s MODULARIZE=1 \
    -s ENVIRONMENT='web' \
    -s NO_DISABLE_EXCEPTION_CATCHING \
    --post-js lifehash.post.js

cp lifehash.d.ts ./lib/
