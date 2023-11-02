#pragma once

#include "lua.hpp"

struct StateDeleter
{
    void operator()(lua_State *L) const
    {
        lua_close(L);
    }
};

