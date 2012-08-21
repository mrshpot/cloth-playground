#include <cstring>
#include <cassert>
#include <string>

#include <lua.hpp>

#include "script.hpp"
#include "world.hpp"


#define PRINT_DEBUG(fmt, ...) printf("DBG: " fmt, ##__VA_ARGS__)

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
}

void Script::reset()
{
}

void Script::update(double dt)
{
}
