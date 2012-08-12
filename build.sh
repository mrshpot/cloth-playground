#!/bin/sh

set -e
cd `dirname $0`

mkdir -p build
cd build
cmake -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
make
