# define the lua core source files
set (LUA_CORE_SRCS lapi.c lcode.c lctype.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c 
	lmem.c lobject.c lopcodes.c lparser.c lstate.c lstring.c ltable.c 
	ltm.c lundump.c lvm.c lzio.c)

# define the lua lib source files
set (LUA_LIB_SRCS lauxlib.c lbaselib.c lbitlib.c lcorolib.c ldblib.c liolib.c 
	lmathlib.c loslib.c lstrlib.c ltablib.c loadlib.c linit.c)

if (UNIX)
  link_libraries(m)
endif (UNIX)

# create the library
add_library (lua ${LUA_LIB_SRCS} ${LUA_CORE_SRCS})

# create the lue executable and link it to the lib
add_executable (luaexec lua.c)
target_link_libraries (luaexec lua)

# name the executable lua just like the library
set_target_properties(luaexec PROPERTIES OUTPUT_NAME lua)

# create the luac thing (I have no clue) and link it
add_executable (luac luac.c)
target_link_libraries (luac lua)

