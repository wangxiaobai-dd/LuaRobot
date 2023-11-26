#pragma once

#include "lua.hpp"
#include <iostream>
#include <string>

struct LuaScopePop
{
    lua_State* lua;
    LuaScopePop(lua_State* L)
        : lua(L)
    {
    }

    LuaScopePop(const LuaScopePop&) = delete;

    LuaScopePop& operator=(const LuaScopePop&) = delete;

    ~LuaScopePop()
    {
        lua_pop(lua, 1);
    }
};

template <typename Type>
Type getLuaValue(lua_State* L, int index)
{
    using T = std::decay_t<Type>;
    if constexpr(std::is_same_v<T, std::string_view>)
    {
        size_t size;
        const char* sz = luaL_checklstring(L, index, &size);
        return std::string_view{sz, size};
    }
    else if constexpr(std::is_same_v<T, std::string>)
    {
        size_t size;
        const char* sz = luaL_checklstring(L, index, &size);
        return std::string{sz, size};
    }
    else if constexpr(std::is_same_v<T, bool>)
    {
        if(!lua_isboolean(L, index))
            luaL_typeerror(L, index, lua_typename(L, LUA_TBOOLEAN));
        return (bool)lua_toboolean(L, index);
    }
    else if constexpr(std::is_integral_v<T>)
    {
        auto v = luaL_checkinteger(L, index);
        luaL_argcheck(L, static_cast<lua_Integer>(static_cast<T>(v)) == v, index, "integer out-of-bounds");
        return static_cast<T>(v);
    }
    else if constexpr(std::is_floating_point_v<T>)
    {
        return static_cast<T>(luaL_checknumber(L, index));
    }
    else
    {
        //  static_assert(false, "unsupport type");
    }
}

template <typename Type>
inline Type getLuaField(lua_State* L, int index, std::string_view key, const Type& def = Type{})
{
    if(index < 0)
    {
        index = lua_gettop(L) + index + 1;
    }

    luaL_checktype(L, index, LUA_TTABLE);
    lua_pushlstring(L, key.data(), key.size());
    LuaScopePop scope{L};
    if(lua_rawget(L, index) <= LUA_TNIL)
        return def;
    return getLuaValue<Type>(L, -1);
}

inline void printLuaStack(lua_State* L)
{
    int stackSize = lua_gettop(L); // 获取堆栈的大小

    std::cout << "Lua stack content: " << stackSize << std::endl;

    for(int i = stackSize; i >= 1; i--)
    {
        int type = lua_type(L, i); // 获取堆栈上索引为 i 的值的类型

        switch(type)
        {
        case LUA_TNIL:
            std::cout << i << ": nil" << std::endl;
            break;
        case LUA_TBOOLEAN:
            std::cout << i << ": " << (lua_toboolean(L, i) ? "true" : "false") << std::endl;
            break;
        case LUA_TNUMBER:
            std::cout << i << ": " << lua_tonumber(L, i) << std::endl;
            break;
        case LUA_TSTRING:
            std::cout << i << ": " << lua_tostring(L, i) << std::endl;
            break;
        // 处理其他类型，如表、函数等
        default:
            std::cout << i << ": " << lua_typename(L, type) << std::endl;
            break;
        }
    }
}

inline int traceback(lua_State* L)
{
    const char* msg = lua_tostring(L, -1);
    if(msg)
    {
        // top == 1, 1:msg
       luaL_traceback(L, L, msg, 0);
       std::cout << lua_tostring(L, -1) << std::endl;
    }
    else
    {
        lua_pushliteral(L, "no error");
    }
    return 1;
}