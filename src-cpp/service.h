#pragma once

#include "common.h"
#include <memory>
#include <string>

class Server;
class Worker;

struct CallbackContext
{
    lua_State* callbackL = nullptr;
};

// 创建服务
struct ServiceConfig
{
    uint32_t workerID = 0;
    std::string luaFile;
    std::string envPath;  // lua 环境变量 搜索模块
};

// 服务间使用消息通信，不能直接访问调用
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

  template<typename Service, typename Message>
    inline void handle_message(Service&& s, Message&& m)
    {
        try
        {
            uint32_t receiver = m.receiver();
            s->dispatch(&m);
            //redirect message
            if (m.receiver() != receiver)
            {
                MOON_ASSERT(!m.broadcast(), "can not redirect broadcast message");
                if constexpr (std::is_rvalue_reference_v<decltype(m)>)
                {
                    s->get_server()->send_message(std::forward<message>(m));
                }
            }
        }
        catch (const std::exception& e)
        {
            CONSOLE_ERROR(s->logger(), "service::handle_message exception: %s", e.what());
        }
    }