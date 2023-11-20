#pragma once

#include "common.h"
#include <memory>

struct CallbackContext
{
    lua_State* callbackL = nullptr;
};

/*
    服务间使用消息通信，不能直接访问调用
*/
class LuaService
{
public:
    LuaService();
    
    static LuaService* getServiceFromL(lua_State* L);
    static int setCallback(lua_State* L);

    void init();
    
private:
    CallbackContext* callback;
    std::unique_ptr<lua_State, StateDeleter> serviceL;
};

struct ServiceConfig
{

};