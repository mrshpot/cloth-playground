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

#include "utils.hpp"
#include "../world.hpp"


static int sphere_new(lua_State *L);
static int sphere_gc(lua_State *L);
static int sphere_index(lua_State *L);
static int sphere_newindex(lua_State *L);
static int sphere_tostring(lua_State *L);

template <> const char *ScriptTypeMetadata<Sphere>::tname = "Cloth.Sphere";

static const luaL_Reg sphere_globals[] = {
    { "new", sphere_new },
    { NULL, NULL }
};

static const luaL_Reg sphere_meta[] = {
    { "__gc", sphere_gc },
    { "__index", sphere_index },
    { "__newindex", sphere_newindex },
    { "__tostring", sphere_tostring },
    { NULL, NULL }
};

void sphere_register(lua_State *L)
{
    luaL_newmetatable(L, ScriptTypeMetadata<Sphere>::tname);
    luaL_register(L, NULL, sphere_meta);
    luaL_register(L, "Sphere", sphere_globals);
    lua_pop(L, 2);
}

static int sphere_new(lua_State *L)
{
    double x = luaL_checknumber(L, 1);
    double y = luaL_checknumber(L, 2);
    double z = luaL_checknumber(L, 3);
    double r = luaL_checknumber(L, 4);

    void *userdata = lua_newuserdata(L, sizeof(Sphere *));
    luaL_getmetatable(L, ScriptTypeMetadata<Sphere>::tname);
    lua_setmetatable(L, -2);

    Sphere *sphere = new Sphere(glm::vec3(x, y, z), r);
    *(Sphere**)userdata = sphere;

    return 1;
}

static int sphere_gc(lua_State *L)
{
    Sphere *sphere = script_checkudata<Sphere>(L, 1);
    delete sphere;
    
    return 0;
}

/// Sphere.mt.__index(sphere, key)
static int sphere_index(lua_State *L)
{
    Sphere *sphere = script_checkudata<Sphere>(L, 1);
    const char *field = luaL_checkstring(L, 2);

    if (strcmp("x", field) == 0)
        lua_pushnumber(L, sphere->origin.x);
    else if (strcmp("y", field) == 0)
        lua_pushnumber(L, sphere->origin.y);
    else if (strcmp("z", field) == 0)
        lua_pushnumber(L, sphere->origin.z);
    else if (strcmp("r", field) == 0)
        lua_pushnumber(L, sphere->r);
    else
        luaL_error(L, "invalid index %s; expected one of x, y, z, r", field);
    
    return 1;
}

static int sphere_newindex(lua_State *L)
{
    Sphere *sphere = script_checkudata<Sphere>(L, 1);
    const char *field = luaL_checkstring(L, 2);
    double value = luaL_checknumber(L, 3);

    if (strcmp("x", field) == 0)
        sphere->origin.x = value;
    else if (strcmp("y", field) == 0)
        sphere->origin.y = value;
    else if (strcmp("z", field) == 0)
        sphere->origin.z = value;
    else if (strcmp("r", field) == 0)
        sphere->r = value;
    else
        luaL_error(L, "invalid index %s; expected one of x, y, z, r", field);
    
    return 0;
}

static int sphere_tostring(lua_State *L)
{
    Sphere *sphere = script_checkudata<Sphere>(L, 1);
    lua_pushfstring(L, "Sphere({%f, %f, %f}, %f)",
                    (lua_Number)sphere->origin.x,
                    (lua_Number)sphere->origin.y,
                    (lua_Number)sphere->origin.z,
                    (lua_Number)sphere->r);
    return 1;
}
