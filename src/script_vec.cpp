/*
 * Copyright (c) 2012, Taras Shpot
 * All rights reserved. Email: mrshpot@gmail.com
 * 
 * This demo is free software; you can redistribute it and/or modify
 * it under the terms of the BSD-style license that is included in the
 * file LICENSE.
 */

#include <cstring>

#include <glm/glm.hpp>
#include <lua.hpp>

#include "script_vec.hpp"


static bool try_get_vec3(lua_State *L, glm::vec3 *out_vec)
{
    if (lua_isnumber(L, -1) &&
        lua_isnumber(L, -2) &&
        lua_isnumber(L, -3))
    {
        if (out_vec != NULL)
        {
            *out_vec = glm::vec3(lua_tonumber(L, -3),
                                 lua_tonumber(L, -2),
                                 lua_tonumber(L, -1));
        }
        return true;
    }
    else
    {
        return false;
    }
}

glm::vec3 script_checkvec(lua_State *L, int index)
{
    if (index < 0)
    {
        index = lua_gettop(L) + index + 1;
    }
    
    luaL_checktype(L, index, LUA_TTABLE);
    glm::vec3 res;
    bool success = false;
    
    // try { x = .., y = .., z = .. }
    lua_getfield(L, index, "x");
    lua_getfield(L, index, "y");
    lua_getfield(L, index, "z");
    success = try_get_vec3(L, &res);
    lua_pop(L, 3);
    if (success)
    {
        return res;
    }

    // try { x, y, z }
    lua_rawgeti(L, index, 1);
    lua_rawgeti(L, index, 2);
    lua_rawgeti(L, index, 3);
    success = try_get_vec3(L, &res);
    lua_pop(L, 3);
    if (success)
    {
        return res;
    }

    luaL_argerror(L, index, "vec3 in form {x, y, z} or {x=.., y=.., z=..} expected");
    return res;
}
