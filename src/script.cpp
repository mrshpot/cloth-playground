/*
 * Copyright (c) 2012, Taras Shpot
 * All rights reserved. Email: mrshpot@gmail.com
 * 
 * This demo is free software; you can redistribute it and/or modify
 * it under the terms of the BSD-style license that is included in the
 * file LICENSE.
 */

#include <cstring>
#include <cassert>
#include <string>

#include <lua.hpp>

#include "script.hpp"
#include "script_detail.hpp"
#include "script_sphere.hpp"
#include "script_plane.hpp"
#include "script_collection.hpp"
#include "world.hpp"


#define PRINT_DEBUG(fmt, ...) printf("DBG: " fmt, ##__VA_ARGS__)
#define DEBUG_CHECKSTACK() do {                     \
        int tmp__top = lua_gettop(m_lua);               \
        if (tmp__top != 0) {                            \
            PRINT_DEBUG("%s:%d: %d items in stack\n",       \
                        __FILE__, __LINE__, tmp__top);      \
        }                                                   \
    } while (false)

class ScriptImpl
{
    lua_State *m_lua;
    World &m_world;
    
public:
    ScriptImpl(World &world);
    ~ScriptImpl();

    bool load(const char *fname, std::string *error_msg);
    
    void init();
    void reset();
    void update(double dt);
};

ScriptImpl::ScriptImpl(World &world)
    : m_world(world)
{
    m_lua = luaL_newstate();
    assert(m_lua != NULL);
    luaL_openlibs(m_lua);
    DEBUG_CHECKSTACK();

    sphere_register(m_lua);
    DEBUG_CHECKSTACK();

    plane_register(m_lua);
    DEBUG_CHECKSTACK();
    
    collection_register(m_lua);
    DEBUG_CHECKSTACK();
    
    collection_new<Sphere>(m_lua, world.spheres);
    lua_setglobal(m_lua, "spheres");
    DEBUG_CHECKSTACK();
    collection_new<Plane>(m_lua, world.planes);
    lua_setglobal(m_lua, "planes");
    DEBUG_CHECKSTACK();
}

ScriptImpl::~ScriptImpl()
{
    lua_close(m_lua);
    m_lua = NULL;
}

bool ScriptImpl::load(const char *fname, std::string *error_msg)
{
    if (luaL_dofile(m_lua, fname) == 0)
    {
        return true;
    }
    else
    {
        if (error_msg != NULL)
        {
            size_t msg_len;
            const char *msg = lua_tolstring(m_lua, -1, &msg_len);
            assert(msg != NULL);
            *error_msg = std::string(msg, msg_len);
        }
        lua_pop(m_lua, 1);
        return false;
    }
}

void ScriptImpl::init()
{
    lua_getglobal(m_lua, "init");
    if (lua_isnil(m_lua, -1))
    {
        lua_pop(m_lua, 1);
        return;
    }
    else
    {
        if (lua_pcall(m_lua, 0, 0, 0) != 0)
        {
            fprintf(stderr, "%s\n", lua_tostring(m_lua, -1));
            lua_pop(m_lua, 1);
        }
    }
}

void ScriptImpl::reset()
{
}

void ScriptImpl::update(double dt)
{
    lua_getglobal(m_lua, "update");
    if (lua_isnil(m_lua, -1))
    {
        lua_pop(m_lua, 1);
        return;
    }
    else
    {
        lua_pushnumber(m_lua, dt);
        if (lua_pcall(m_lua, 1, 0, 0) != 0)
        {
            fprintf(stderr, "%s\n", lua_tostring(m_lua, -1));
            lua_pop(m_lua, 1);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

Script::Script(World &world)
    : m(new ScriptImpl(world))
{
}

Script::~Script()
{
    delete m;
    m = NULL;
}

bool Script::load(const char *fname, std::string *error_msg)
{
    return m->load(fname, error_msg);
}

void Script::init()
{
    m->init();
}

void Script::reset()
{
    m->reset();
}

void Script::update(double dt)
{
    m->update(dt);
}
