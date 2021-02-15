#!/usr/bin/env bash

set +e

source tasks.sh

configure
clean
build
check
