cmake_minimum_required(VERSION 2.6)

set(TARGET verlet-cloth)

project(${TARGET})

add_executable(${TARGET}
  src/main.cpp src/cloth.cpp)
target_link_libraries(${TARGET} glut GLU GL GLEW rt m)
