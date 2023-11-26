
#include "service.h"
#include "lauxlib.h"
#include "lua_utility.h"
#include "server.h"
#include "worker.h"
#include <cassert>
#include <cstdint>

extern "C" void registerLibs(lua_State* L);

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

void LuaService::setContext(Server* _server, Worker* _worker)
{
    server = _server;
    worker = _worker;
}

static int loadLuaFile(lua_State* L)
{
    const ServiceOption* option = (const ServiceOption*)lua_touserdata(L, 1);

    luaL_openlibs(L);
    registerLibs(L);

    // 设置环境变量 package.path
    if((luaL_dostring(L, option->envPath.data())) != LUA_OK)
        return 1;

    // 解析luaFile
    if(auto ret = luaL_loadfile(L, option->luaFile.data()); ret != LUA_OK)
    {
        std::cout << lua_tostring(L, -1) << std::endl;
        std::cout << "luaL_loadfile err:" << ret << std::endl;
        return 1;
    }

    lua_call(L, 0, 0);
    return 0;
};

bool LuaService::init(const ServiceOption& option)
{
    lua_State* L = serviceL.get();

    lua_gc(L, LUA_GCSTOP, 0);
    lua_gc(L, LUA_GCGEN, 0, 0);

    lua_pushcfunction(L, traceback);
    int traceN = lua_gettop(L);

    std::cout << "trace_fn:" << traceN << std::endl;

    lua_pushcfunction(L, loadLuaFile);
    lua_pushlightuserdata(L, (void*)&option);

    // lua_gettop(L) > 1 有错误  top == 2 2:err info
    if(lua_pcall(L, 1, LUA_MULTRET, 1) != LUA_OK || lua_gettop(L) > 1)
        return false;

    lua_pop(L, 1); // traceback

    lua_gc(L, LUA_GCRESTART, 0);

    assert(lua_gettop(L) == 0);

    return true;
}

/*
void lua_service::dispatch(message* msg)
{
    if(!ok())
        return;
    if(cb_ctx == nullptr)
    {
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
        if(r == LUA_OK)
        {
            return;
        }

        std::string error;

        switch(r)
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

        if(msg->sessionid() >= 0)
        {
            logger()->logstring(true, moon::LogLevel::Error, error, id());
        }
        else
        {
            msg->set_sessionid(-msg->sessionid());
            server_->response(msg->sender(), error, msg->sessionid(), PTYPE_ERROR);
        }
    }
    catch(const std::exception& e)
    {
        luaL_traceback(L, L, e.what(), 1);
        const char* trace = lua_tostring(L, -1);
        if(nullptr == trace)
        {
            trace = "";
        }
        CONSOLE_ERROR(logger(), "dispatch:\n%s", trace);
        lua_pop(L, 1);
    }
}
*/