/*
 * Copyright (c) 2012, Taras Shpot
 * All rights reserved. Email: mrshpot@gmail.com
 * 
 * This demo is free software; you can redistribute it and/or modify
 * it under the terms of the BSD-style license that is included in the
 * file LICENSE.
 */

#ifndef SCRIPT_UTILS_HPP__INCLUDED
#define SCRIPT_UTILS_HPP__INCLUDED


struct lua_State;

template <typename T>
struct ScriptTypeMetadata
{
    static const char *tname;
};

template <typename T>
T *script_checkudata(lua_State *L, int idx)
{
    void *userdata = luaL_checkudata(L, idx, ScriptTypeMetadata<T>::tname);
    return *(T**)userdata;
}

template <typename T>
T *script_checkudata(lua_State *L, int idx, const char *tname)
{
    void *userdata = luaL_checkudata(L, idx, tname);
    return *(T**)userdata;
}

#endif // SCRIPT_UTILS_HPP__INCLUDED
