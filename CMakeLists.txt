cmake_minimum_required(VERSION 2.6)

set(TARGET verlet-cloth)

project(${TARGET})

IF("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  SET(PLATFORM_POSIX Yes)
  SET(LIBS glut GLU GL GLEW rt m)
ELSE("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  SET(PLATFORM_WINDOWS Yes)
  SET(LIBS glut opengl32 glu32 glew32)
ELSE()
  MESSAGE(FATAL_ERROR "Unrecognized platform: ${CMAKE_SYSTEM_NAME}")
  RETURN()
ENDIF()
  

configure_file(src/platform.hpp.in src/platform.hpp)
include_directories("${CMAKE_CURRENT_BINARY_DIR}/src")
add_executable(${TARGET}
  src/main.cpp src/cloth.cpp src/w32_time.cpp src/posix_time.cpp)
target_link_libraries(${TARGET} ${LIBS})
