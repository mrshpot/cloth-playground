#!/bin/sh

set -e
cd `dirname $0`

dst=build
mkdir -p "$dst"
cd "$dst"

cmake -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
make
