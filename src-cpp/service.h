#pragma once

#include "common.h"
#include <memory>

class Server;
class Worker;

struct CallbackContext
{
    lua_State* callbackL = nullptr;
};

/*
    �����ʹ����Ϣͨ�ţ�����ֱ�ӷ��ʵ���
*/
class LuaService
{
public:
    LuaService();
    
    static LuaService* getServiceFromL(lua_State* L);
    static int setCallback(lua_State* L);

    void init();
    void setContext(Server* _server, Worker* _worker);

private:
    CallbackContext* callback;
    std::unique_ptr<lua_State, StateDeleter> serviceL;
    Server* server = nullptr;
    Worker* worker = nullptr;
    uint32_t serviceID = 0;
};

