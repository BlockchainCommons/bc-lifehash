#!/bin/zsh

set +e

source tasks.sh

configure
clean
build
check
