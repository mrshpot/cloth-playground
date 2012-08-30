project (LUA C)

# the following two variables are defined for the use of packages 
# that wish to link or compile against lua
set (LUA_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src")
set (LUA_LIBRARIES lua)

add_subdirectory (src)
