# Origin: http://blog.beuc.net/posts/Cross-compiling_with_CMake/

set(CMAKE_SYSTEM_NAME Windows)
include(CMakeForceCompiler)
if("${GNU_HOST}" STREQUAL "")
  set(GNU_HOST i486-mingw32)
endif()

cmake_force_c_compiler(${GNU_HOST}-gcc GNU)
cmake_force_cxx_compiler(${GNU_HOST}-g++ GNU)
set(CMAKE_RC_COMPILER ${GNU_HOST}-windres)
