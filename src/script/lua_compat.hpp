/*
 * Copyright (c) 2012, Taras Shpot
 * All rights reserved. Email: mrshpot@gmail.com
 * 
 * This demo is free software; you can redistribute it and/or modify
 * it under the terms of the BSD-style license that is included in the
 * file LICENSE.
 */
 
#ifndef LUA_COMPAT_HPP__INCLUDED
#define LUA_COMPAT_HPP__INCLUDED

struct lua_State;
struct luaL_Reg;

void script_register(lua_State *L, const luaL_Reg *l);

#endif // LUA_COMPAT_HPP__INCLUDED