cmake_minimum_required(VERSION 2.6)

set(TARGET cloth-playground)
project(${TARGET})

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  set(PLATFORM_POSIX Yes)
else("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  set(PLATFORM_WINDOWS Yes)
else()
  message(FATAL_ERROR "Unrecognized platform: ${CMAKE_SYSTEM_NAME}")
  return()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake-modules/")

set(VENDORS ${PROJECT_SOURCE_DIR}/3rdparty)
set(GLM_INCLUDE_DIR ${VENDORS}/glm-0.9.3.4)
set(LUA_SOURCE_DIR ${VENDORS}/lua-5.2.1)
set(LUA_INCLUDE_DIR ${LUA_SOURCE_DIR}/src)

find_package(OpenGL)
find_package(GLUT)
find_package(GLEW)

add_subdirectory(${LUA_SOURCE_DIR})
add_subdirectory(src)
