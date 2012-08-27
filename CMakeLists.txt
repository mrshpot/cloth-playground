cmake_minimum_required(VERSION 2.6)

set(TARGET cloth-playground)

project(${TARGET})

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  set(PLATFORM_POSIX Yes)
  set(LIBS lua glut GLU GL GLEW rt m)
else("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  set(PLATFORM_WINDOWS Yes)
  set(LIBS lua glut opengl32 glu32 glew32)
else()
  message(FATAL_ERROR "Unrecognized platform: ${CMAKE_SYSTEM_NAME}")
  return()
endif()
  

configure_file(src/platform.hpp.in src/platform.hpp)
include_directories("${CMAKE_CURRENT_BINARY_DIR}/src")
add_executable(${TARGET}
  src/main.cpp src/cloth.cpp
  src/script.cpp src/script_vec.cpp src/script_plane.cpp src/script_sphere.cpp src/script_collection.cpp
  src/w32_time.cpp src/posix_time.cpp)
target_link_libraries(${TARGET} ${LIBS})
