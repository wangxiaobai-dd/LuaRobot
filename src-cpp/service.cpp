
#include "service.h"
#include "server.h"
#include "worker.h"
#include <cstdint>

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

void LuaService::setContext(Server* _server, Worker* _worker)
{
    server = _server;
    worker = _worker;
}

static int protect_init(lua_State* L)
{
    const moon::service_conf* conf = (const moon::service_conf*)lua_touserdata(L, 1);

    luaL_openlibs(L);
    open_custom_libs(L);

    if ((luaL_loadfile(L, conf->source.data())) != LUA_OK)
        return 1;

    if ((luaL_dostring(L, conf->params.data())) != LUA_OK)
        return 1;

    lua_call(L, 1, 0);
    return 0;
};

bool lua_service::init(const moon::service_conf& conf)
{
    mem_limit = conf.memlimit;
    name_ = conf.name;

    logger()->logstring(true, moon::LogLevel::Info, moon::format("[WORKER %u] new service [%s]", worker_->id(), name().data()), id());

    lua_State* L = lua_.get();

    lua_gc(L, LUA_GCSTOP, 0);
    lua_gc(L, LUA_GCGEN, 0, 0);


    lua_pushcfunction(L, traceback);
    int trace_fn = lua_gettop(L);

    lua_pushcfunction(L, protect_init);
    lua_pushlightuserdata(L,(void*)&conf);

    if (lua_pcall(L, 1, LUA_MULTRET, trace_fn) != LUA_OK || lua_gettop(L) > 1)
    {
        CONSOLE_ERROR(logger(), "new_service lua_error:\n%s.", lua_tostring(L, -1));
        return false;
    }

	lua_pop(L, 1);

    if (unique_ && !server_->set_unique_service(name_, id_))
    {
        CONSOLE_ERROR(logger(), "duplicate unique service name '%s'.", name_.data());
        return false;
    }

    lua_gc(L, LUA_GCRESTART, 0);

    assert(lua_gettop(L) == 0);

    ok_ = true;

    return ok_;
}


void lua_service::dispatch(message* msg)
{
    if (!ok())
        return;
    if (cb_ctx == nullptr) {
        logger()->logstring(true, moon::LogLevel::Error, "callback not init", id());
        return;
    }
    lua_State* L = cb_ctx->L;
    try
    {
        int trace = 1;
        lua_pushvalue(L, 2);

        lua_pushlightuserdata(L, msg);
        lua_pushinteger(L, msg->type());

        int r = lua_pcall(L, 2, 0, trace);
        if (r == LUA_OK)
        {
            return;
        }

        std::string error;

        switch (r)
        {
        case LUA_ERRRUN:
            error = moon::format("dispatch %s error:\n%s", name().data(), lua_tostring(L, -1));
            break;
        case LUA_ERRMEM:
            error = moon::format("dispatch %s memory error", name().data());
            break;
        case LUA_ERRERR:
            error = moon::format("dispatch %s error in error", name().data());
            break;
        };

        lua_pop(L, 1);

        if (msg->sessionid() >= 0)
        {
            logger()->logstring(true, moon::LogLevel::Error, error, id());
        }
        else
        {
            msg->set_sessionid(-msg->sessionid());
            server_->response(msg->sender(), error, msg->sessionid(), PTYPE_ERROR);
        }
    }
    catch (const std::exception& e)
    {
        luaL_traceback(L, L, e.what(), 1);
        const char* trace = lua_tostring(L, -1);
        if (nullptr == trace)
        {
            trace = "";
        }
        CONSOLE_ERROR(logger(), "dispatch:\n%s", trace);
        lua_pop(L, 1);
    }
}
