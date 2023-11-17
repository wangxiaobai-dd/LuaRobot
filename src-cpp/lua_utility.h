
// from moon

#pragma once
#include "../third/lua/lua.h"
#include "../third/lua/lua.hpp"
#include <string>

struct lua_scope_pop
{
    lua_State *lua;
    lua_scope_pop(lua_State *L)
        : lua(L)
    {
    }

    lua_scope_pop(const lua_scope_pop &) = delete;

    lua_scope_pop &operator=(const lua_scope_pop &) = delete;

    ~lua_scope_pop()
    {
        lua_pop(lua, 1);
    }
};

template <typename Type>
Type lua_check(lua_State *L, int index)
{
    using T = std::decay_t<Type>;
    if constexpr (std::is_same_v<T, std::string_view>)
    {
        size_t size;
        const char *sz = luaL_checklstring(L, index, &size);
        return std::string_view{sz, size};
    }
    else if constexpr (std::is_same_v<T, std::string>)
    {
        size_t size;
        const char *sz = luaL_checklstring(L, index, &size);
        return std::string{sz, size};
    }
    else if constexpr (std::is_same_v<T, bool>)
    {
        if (!lua_isboolean(L, index))
            luaL_typeerror(L, index, lua_typename(L, LUA_TBOOLEAN));
        return (bool)lua_toboolean(L, index);
    }
    else if constexpr (std::is_integral_v<T>)
    {
        auto v = luaL_checkinteger(L, index);
        luaL_argcheck(L, static_cast<lua_Integer>(static_cast<T>(v)) == v, index, "integer out-of-bounds");
        return static_cast<T>(v);
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        return static_cast<T>(luaL_checknumber(L, index));
    }
    else
    {
      //  static_assert(false, "unsupport type");
    }
}

template <typename Type>
inline Type lua_opt_field(lua_State *L, int index, std::string_view key, const Type &def = Type{})
{
    if (index < 0)
    {
        index = lua_gettop(L) + index + 1;
    }

    luaL_checktype(L, index, LUA_TTABLE);
    lua_pushlstring(L, key.data(), key.size());
    lua_scope_pop scope{L};
    if (lua_rawget(L, index) <= LUA_TNIL)
        return def;
    return lua_check<Type>(L, -1);
}