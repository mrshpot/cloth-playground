/*
 * Copyright (c) 2012, Taras Shpot
 * All rights reserved. Email: mrshpot@gmail.com
 * 
 * This demo is free software; you can redistribute it and/or modify
 * it under the terms of the BSD-style license that is included in the
 * file LICENSE.
 */

#include <vector>
#include <string>
#include <cassert>

#include <lua.hpp>

#include "utils.hpp"
#include "../world.hpp"


class ScriptCollectionBase
{
public:
    virtual ~ScriptCollectionBase() {}

    virtual void gc(lua_State *L) = 0;
    virtual void append(lua_State *L) = 0;
    virtual void erase(lua_State *L) = 0;
    virtual void index(lua_State *L) = 0;
};

template <> const char *ScriptTypeMetadata<ScriptCollectionBase>::tname = "Cloth.Collection";

#define DEFINE_ACTION(name, outargs)                        \
    static int collection_##name(lua_State *L)              \
    {                                                       \
        ScriptCollectionBase *collection =                  \
            script_checkudata<ScriptCollectionBase>(L, 1);  \
        collection->name(L);                                \
        return outargs;                                     \
    }

DEFINE_ACTION(append, 1)
DEFINE_ACTION(erase, 0)
DEFINE_ACTION(index, 1)

#undef DEFINE_ACTION

static int collection_gc(lua_State *L)
{
    ScriptCollectionBase *collection = \
        script_checkudata<ScriptCollectionBase>(L, 1);
    collection->gc(L);
    delete collection;
    return 0;
}

static const luaL_Reg collection_meta[] = {
    { "__gc", collection_gc },
    { NULL, NULL }
};

static const luaL_Reg collection_fields[] = {
    { "append", collection_append },
    { "erase", collection_erase },
    { "get", collection_index },
    { NULL, NULL }
};

void collection_register(lua_State *L)
{
    luaL_newmetatable(L, ScriptTypeMetadata<ScriptCollectionBase>::tname);
    luaL_register(L, NULL, collection_meta);
    luaL_register(L, NULL, collection_fields);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);
}

template <typename T>
class ScriptCollection : public ScriptCollectionBase
{
    std::vector<T*> &m_collection;
    std::vector<int> m_refs;
    
public:
    ScriptCollection(std::vector<T*> &collection)
        : m_collection(collection)
    {
        assert(collection.empty());
    }

    virtual void gc(lua_State *L)
    {
        for (std::vector<int>::iterator it = m_refs.begin();
             it != m_refs.end(); ++it)
        {
            luaL_unref(L, LUA_REGISTRYINDEX, *it);
        }
    }

    virtual void append(lua_State *L)
    {
        T *item = script_checkudata<T>(L, -1);
        m_refs.push_back(luaL_ref(L, LUA_REGISTRYINDEX));
        m_collection.push_back(item);
        
        assert(m_refs.size() == m_collection.size());
    }

    virtual void erase(lua_State *L)
    {
        assert(m_refs.size() == m_collection.size());
        
        int idx = luaL_checkint(L, -1);
        if (idx < 1 || idx > m_collection.size())
        {
            luaL_error(L, "index %d out of range", idx);
            return;
        }
        idx -= 1;

        // the item itself should be eventually GC'd by Lua
        luaL_unref(L, LUA_REGISTRYINDEX, m_refs[idx]);
        m_refs.erase(m_refs.begin() + idx);
        m_collection.erase(m_collection.begin() + idx);
    }

    virtual void index(lua_State *L)
    {
        int idx = luaL_checkint(L, 2);
        if (idx < 1 || idx > m_collection.size())
        {
            luaL_error(L, "index %d out of range", idx);
            return;
        }
        idx -= 1;

        lua_rawgeti(L, LUA_REGISTRYINDEX, m_refs[idx]);
    }

private:
    ScriptCollection(const ScriptCollection &);
};

template <typename T>
void collection_new(lua_State *L, std::vector<T*> &collection)
{
    // TODO: call via lua_pcall(), pass collection as light userdata
    void *userdata = lua_newuserdata(L, sizeof(ScriptCollectionBase*));
    luaL_getmetatable(L, ScriptTypeMetadata<ScriptCollectionBase>::tname);
    lua_setmetatable(L, -2);

    ScriptCollection<T> *res = new ScriptCollection<T>(collection);
    *(ScriptCollection<T> **)userdata = res;
}  

// instantiate templates

template void collection_new<Sphere>(lua_State *L, std::vector<Sphere*> &);
template void collection_new<Plane>(lua_State *L, std::vector<Plane*> &);
