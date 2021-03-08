#!/bin/zsh

PROJ_ROOT=${PWD}
SRC_ROOT=${PROJ_ROOT}/src
LIB_ROOT=${PROJ_ROOT}/liblifehash

TARGET_OPT=
# TARGET_OPT="-target apple-darwin-x86_64"
# TARGET_OPT="-target apple-darwin-arm64"

# OPTIMIZE_OPT="--debug -O0"
OPTIMIZE_OPT=-O3


if [[ "$OSTYPE" == "linux"* ]]; then
    export CFLAGS="${TARGET_OPT} ${OPTIMIZE_OPT}"
    export CXXFLAGS="${CFLAGS} -std=c++17"
    export CPPFLAGS=
    export LDFLAGS="${TARGET_OPT}"
else
    export CFLAGS="${TARGET_OPT} ${OPTIMIZE_OPT} -fdiagnostics-absolute-paths"
    export CXXFLAGS="${CFLAGS} -std=c++17 -stdlib=libc++"
    export CPPFLAGS=
    export LDFLAGS="${TARGET_OPT} -lstdc++"
fi

clean()
(
    make clean
)

configure()
(
    ./configure --prefix=${PREFIX:-/usr/local}
    clean
)

build()
(
    make liblifehash
    make test
    dsymutil test/test
)

check()
(
    make check
)

install()
(
    make install
)

uninstall()
(
    make uninstall
)
