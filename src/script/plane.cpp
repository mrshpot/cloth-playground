/*
 * Copyright (c) 2012, Taras Shpot
 * All rights reserved. Email: mrshpot@gmail.com
 * 
 * This demo is free software; you can redistribute it and/or modify
 * it under the terms of the BSD-style license that is included in the
 * file LICENSE.
 */

#include <cstring>

#include <lua.hpp>

#include "lua_compat.hpp"
#include "utils.hpp"
#include "vec.hpp"
#include "../world.hpp"


static int plane_new(lua_State *L);
static int plane_gc(lua_State *L);
static int plane_index(lua_State *L);
static int plane_newindex(lua_State *L);
static int plane_tostring(lua_State *L);

static int plane_from_triangle(lua_State *L);

template <> const char *ScriptTypeMetadata<Plane>::tname = "Cloth.Plane";

static const luaL_Reg plane_globals[] = {
    { "new", plane_new },
    { NULL, NULL }
};

static const luaL_Reg plane_meta[] = {
    { "__gc", plane_gc },
    { "__index", plane_index },
    { "__newindex", plane_newindex },
    { "__tostring", plane_tostring },
    { NULL, NULL }
};

static const luaL_Reg plane_fields[] = {
    { "from_triangle", plane_from_triangle },
    { NULL, NULL }
};

void plane_register(lua_State *L)
{
    luaL_newmetatable(L, ScriptTypeMetadata<Plane>::tname);
    script_register(L, plane_meta);
    lua_newtable(L);
    script_register(L, plane_globals);
    lua_setglobal(L, "Plane");
    lua_pop(L, 1);
}

static int plane_new(lua_State *L)
{
    double a = luaL_checknumber(L, 1);
    double b = luaL_checknumber(L, 2);
    double c = luaL_checknumber(L, 3);
    double d = luaL_checknumber(L, 4);

    void *userdata = lua_newuserdata(L, sizeof(Plane *));
    luaL_getmetatable(L, ScriptTypeMetadata<Plane>::tname);
    lua_setmetatable(L, -2);

    Plane *plane = new Plane(glm::vec3(a, b, c), d);
    *(Plane**)userdata = plane;

    return 1;
}

static int plane_gc(lua_State *L)
{
    Plane *plane = script_checkudata<Plane>(L, 1);
    delete plane;
    
    return 0;
}

/// Plane.mt.__index(plane, key)
static int plane_index(lua_State *L)
{
    Plane *plane = script_checkudata<Plane>(L, 1);
    const char *field = luaL_checkstring(L, 2);

    // fields
    if (strcmp("a", field) == 0)
        lua_pushnumber(L, plane->n.x);
    else if (strcmp("b", field) == 0)
        lua_pushnumber(L, plane->n.y);
    else if (strcmp("c", field) == 0)
        lua_pushnumber(L, plane->n.z);
    else if (strcmp("d", field) == 0)
        lua_pushnumber(L, plane->d);
    // methods
    else if (strcmp("from_triangle", field) == 0)
        lua_pushcfunction(L, plane_from_triangle);
    else
        luaL_error(L, "invalid index %s; expected one of a, b, c, d", field);
    
    return 1;
}

static int plane_newindex(lua_State *L)
{
    Plane *plane = script_checkudata<Plane>(L, 1);
    const char *field = luaL_checkstring(L, 2);
    double value = luaL_checknumber(L, 3);

    if (strcmp("a", field) == 0)
        plane->n.x = value;
    else if (strcmp("b", field) == 0)
        plane->n.y = value;
    else if (strcmp("c", field) == 0)
        plane->n.z = value;
    else if (strcmp("d", field) == 0)
        plane->d = value;
    else
        luaL_error(L, "invalid index %s; expected one of a, b, c, d", field);
    
    return 0;
}

static int plane_tostring(lua_State *L)
{
    Plane *plane = script_checkudata<Plane>(L, 1);
    lua_pushfstring(L, "Plane({%f, %f, %f}, %f)",
                    (lua_Number)plane->n.x,
                    (lua_Number)plane->n.y,
                    (lua_Number)plane->n.z,
                    (lua_Number)plane->d);
    return 1;
}

static int plane_from_triangle(lua_State *L)
{
    Plane *plane = script_checkudata<Plane>(L, 1);
    glm::vec3 a = script_checkvec(L, 2);
    glm::vec3 b = script_checkvec(L, 3);
    glm::vec3 c = script_checkvec(L, 4);

    *plane = Plane(a, b, c);
    lua_settop(L, 1);

    return 1;
}
