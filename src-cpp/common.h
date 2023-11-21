#pragma once

#include "../third/lua/lua.hpp"

constexpr uint32_t WORKER_ID_SHIFT = 24;

struct StateDeleter
{
    void operator()(lua_State *L) const
    {
        lua_close(L);
    }
};

inline int traceback(lua_State *L)
{
    const char *msg = lua_tostring(L, 1);
    if (msg)
        luaL_traceback(L, L, msg, 1);
    else
    {
        lua_pushliteral(L, "no error");
    }
    return 1;
}

