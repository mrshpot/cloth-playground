# Origin: http://blog.beuc.net/posts/Cross-compiling_with_CMake/

SET(CMAKE_SYSTEM_NAME Windows)
include(CMakeForceCompiler)
IF("${GNU_HOST}" STREQUAL "")
  SET(GNU_HOST i486-mingw32)
ENDIF()

CMAKE_FORCE_C_COMPILER(${GNU_HOST}-gcc GNU)
CMAKE_FORCE_CXX_COMPILER(${GNU_HOST}-g++ GNU)
SET(CMAKE_RC_COMPILER ${GNU_HOST}-windres)
