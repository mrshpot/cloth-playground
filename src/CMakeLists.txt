if(PLATFORM_POSIX)
  set(LIBS lua glut GLU GL GLEW rt m)
else(PLATFORM_WINDOWS)
  set(LIBS lua ${GLUT_glut_LIBRARY} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${GLEW_LIBRARY})
else()
  message(FATAL_ERROR "Unrecognized platform: ${CMAKE_SYSTEM_NAME}")
  return()
endif()

configure_file(platform.hpp.in platform.hpp)
include_directories("." "${CMAKE_CURRENT_BINARY_DIR}" "${GLM_INCLUDE_DIR}" "${GLUT_INCLUDE_DIR}" "${GLEW_INCLUDE_PATH}" "${LUA_INCLUDE_DIR}")
add_executable(${TARGET}
  main.cpp cloth.cpp
  script.cpp script/lua_compat.cpp script/vec.cpp script/plane.cpp script/sphere.cpp script/collection.cpp
  w32_time.cpp posix_time.cpp)
target_link_libraries(${TARGET} ${LIBS})
