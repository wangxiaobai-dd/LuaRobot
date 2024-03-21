
#include "lua.hpp"
#include "lua_utility.h"
#include "server.h"
#include "service.h"

// https://blog.codingnow.com/2022/04/lua_binding_callback.html#more
// 设置消息处理回调
static int setCallback(lua_State* L)
{
    LuaService* service = LuaService::getServiceFromL(L);
    luaL_checktype(L, 1, LUA_TFUNCTION);
    lua_settop(L, 1);
    CallbackContext* cb = (CallbackContext*)lua_newuserdata(L, sizeof(CallbackContext));
    cb->callbackL = lua_newthread(L); // [-3:lua-callback, -2:userdata-cb, -1:thread]
    // lua_pushcfunction(cb->callbackL, traceback);
    lua_setuservalue(L, -2); // 栈顶的值设置到指定索引处用户数据
    lua_setfield(L, LUA_REGISTRYINDEX, "callback_context");
    lua_xmove(L, cb->callbackL, 1); // from, to, num
    service->setCB(cb);
    return 0;
}

static int newService(lua_State* L)
{
    luaL_checktype(L, 2, LUA_TTABLE);

    LuaService* service = LuaService::getServiceFromL(L);

    auto option = std::make_unique<ServiceOption>();

    option->workerID = getLuaField<uint32_t>(L, 2, "workerid");
    option->luaFile = getLuaField<std::string>(L, 2, "luafile");
    option->envPath = service->getServer()->envPath;

    service->getServer()->newService(std::move(option));
    return 0;
}

// service 即 actor
// 每一个 robot 都是一个 actor
static int fowardToActor(lua_State* L)
{    
    LuaService* S = LuaService::getServiceFromL(L);

    uint32_t receiver = (uint32_t)luaL_checkinteger(L, 1);
    luaL_argcheck(L, receiver > 0, 1, "receiver must > 0");

    S->getServer()->forwardToActor(S->getID(), receiver);
    return 0;
}

static int sendToGameServer(lua_State* L)
{
    return 0;
}

extern "C"
{
    int LUAMOD_API luaopen_core(lua_State* L)
    {
        luaL_Reg l[] = {

            {"callback", setCallback},
            {"new_service", newService},
            {NULL, NULL}};

        luaL_newlib(L, l);
        return 1;
    }
}

#define REGISTER_LIBRARY(name, lua_c_fn) \
    int lua_c_fn(lua_State*);            \
    luaL_requiref(L, name, lua_c_fn, 0); \
    lua_pop(L, 1); /* remove lib */

extern "C"
{
    void registerLibs(lua_State* L)
    {
        REGISTER_LIBRARY("core", luaopen_core);
    }
}
