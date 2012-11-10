set(LIBS lua ${GLUT_glut_LIBRARY} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${GLEW_LIBRARY})
if (PLATFORM_POSIX)
  set(LIBS ${LIBS} rt m)
endif (PLATFORM_POSIX)

configure_file(platform.hpp.in platform.hpp)
include_directories("." "${CMAKE_CURRENT_BINARY_DIR}" "${GLM_INCLUDE_DIR}" "${GLUT_INCLUDE_DIR}" "${GLEW_INCLUDE_PATH}" "${LUA_INCLUDE_DIR}")
add_executable(${TARGET}
  main.cpp cloth.cpp surface.cpp
  script.cpp script/lua_compat.cpp script/vec.cpp script/plane.cpp script/sphere.cpp script/collection.cpp
  w32_time.cpp posix_time.cpp)
target_link_libraries(${TARGET} ${LIBS})
