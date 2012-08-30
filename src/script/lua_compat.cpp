/*
 * Copyright (c) 2012, Taras Shpot
 * All rights reserved. Email: mrshpot@gmail.com
 * 
 * This demo is free software; you can redistribute it and/or modify
 * it under the terms of the BSD-style license that is included in the
 * file LICENSE.
 */

#include <lua.hpp>

#include "lua_compat.hpp"


void script_register(lua_State *L, const luaL_Reg *l)
{
#if LUA_VERSION_NUM >= 502
    luaL_setfuncs(L, l, 0);
#else
    luaL_register(L, NULL, l);
#endif
}
