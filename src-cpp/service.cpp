
#include "service.h"

LuaService::LuaService()
{
    serviceL = std::make_unique<lua_newstate()>();
}

// https://blog.codingnow.com/2022/04/lua_binding_callback.html#more
// 设置消息处理回调
int LuaService::setCallback(lua_State *L)
{
    lua_service *S = lua_service::get(L);
    luaL_checktype(L, 1, LUA_TFUNCTION);
    lua_settop(L, 1);
    callback_context *cb_ctx = (callback_context *)lua_newuserdata(L, sizeof(callback_context)); // L [-0, +1]
    cb_ctx->L = lua_newthread(L);                                                                // L [-0, +1]
    lua_pushcfunction(cb_ctx->L, traceback);                                                     // cb_ctx->L [-0, +1]
    lua_setuservalue(L, -2);   //栈顶的值设置到指定索引处用户数据                                                // 将-2的值 设置为栈顶                       // L [-1, +0]  associate cb_ctx->L to the userdata
    lua_setfield(L, LUA_REGISTRYINDEX, "callback_context");                                      // L [-1, +0] ref userdata(cb_ctx), avoid free by GC
    lua_xmove(L, cb_ctx->L, 1);                                                                  // L [-1, +0], cb_ctx->L [-0, +1] move callback-function to cb_ctx->L, cb_ctx->L's top=2
    S->cb_ctx = cb_ctx;
    return 0;
}