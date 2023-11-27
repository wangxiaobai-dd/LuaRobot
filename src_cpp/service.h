#pragma once

#include "common.h"
#include <memory>
#include <string>


class Server;
class Worker;



// 服务间使用消息通信，不能直接访问调用
class LuaService
{
public:
    LuaService();

    static LuaService* getServiceFromL(lua_State* L);

    bool init(const ServiceOption& option);
    void setContext(Server* _server, Worker* _worker);
    void setCB(CallbackContext* cb) { callback = cb; }
    Server* getServer(){ return server; }

private:
    CallbackContext* callback = nullptr;
    std::unique_ptr<lua_State, StateDeleter> serviceL;
    Server* server = nullptr;
    Worker* worker = nullptr;
    uint32_t serviceID = 0;
};

/*
template <typename Service, typename Message>
inline void handle_message(Service&& s, Message&& m)
{

    uint32_t receiver = m.receiver();
    s->dispatch(&m);
    // redirect message
    if(m.receiver() != receiver)
    {
        MOON_ASSERT(!m.broadcast(), "can not redirect broadcast message");
        if constexpr(std::is_rvalue_reference_v<decltype(m)>)
        {
            s->get_server()->send_message(std::forward<message>(m));
        }
    }
}
*/