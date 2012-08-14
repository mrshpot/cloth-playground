#!/bin/sh

set -e
cd `dirname $0`

dst=build-cross
mkdir -p "$dst"
cd "$dst"

GNU_HOST=i486-mingw32
cmake -DGNU_HOST=${GNU_HOST} -DCMAKE_TOOLCHAIN_FILE=../mingw-cross.cmake -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
make

mkdir -p dlls
for lib in libgcc_s_sjlj-1.dll libstdc++-6.dll ; do
	cp -f "/usr/${GNU_HOST}/lib/${lib}" dlls/
done

for lib in glew32.dll libglut-0.dll ; do
	cp -f "/usr/${GNU_HOST}/bin/${lib}" dlls/
done
	
