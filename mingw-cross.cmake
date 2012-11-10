# Origin: http://blog.beuc.net/posts/Cross-compiling_with_CMake/
# Also: http://www.cmake.org/Wiki/CmakeMingw

set(CMAKE_SYSTEM_NAME Windows)
if("${GNU_HOST}" STREQUAL "")
  set(GNU_HOST i486-mingw32)
endif()

set(CMAKE_C_COMPILER ${GNU_HOST}-gcc)
set(CMAKE_CXX_COMPILER ${GNU_HOST}-g++)
set(CMAKE_RC_COMPILER ${GNU_HOST}-windres)

# here is the target environment located
set(CMAKE_FIND_ROOT_PATH /usr/${GNU_HOST})

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
