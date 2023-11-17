#include <cstdint>
#include "service.h"

LuaService::LuaService()
{
    serviceL = std::unique_ptr<lua_State, StateDeleter>(luaL_newstate());
}

LuaService* LuaService::getServiceFromL(lua_State* L)
{
    std::intptr_t v = 0;
    memcpy(&v, lua_getextraspace(L), LUA_EXTRASPACE);
    return reinterpret_cast<LuaService*>(v);
}

// https://blog.codingnow.com/2022/04/lua_binding_callback.html#more
// 设置消息处理回调
int LuaService::setCallback(lua_State *L)
{
    LuaService* service = LuaService::getServiceFromL(L);
    luaL_checktype(L, 1, LUA_TFUNCTION);
    lua_settop(L, 1);
    CallbackContext* cb = (CallbackContext *)lua_newuserdata(L, sizeof(CallbackContext));
    cb->callbackL = lua_newthread(L); // [-3:lua-callback, -2:userdata-cb, -1:thread]
    //lua_pushcfunction(cb->callbackL, traceback);
    lua_setuservalue(L, -2);    // 栈顶的值设置到指定索引处用户数据
    lua_setfield(L, LUA_REGISTRYINDEX, "callback_context");
    lua_xmove(L, cb->callbackL, 1); // from, to, num                                                             
    service->callback = cb;
    return 0;
}

void LuaService::init()
{
    
}