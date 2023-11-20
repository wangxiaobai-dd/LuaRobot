#pragma once
/*
    actor 模型，每个线程 一或多 服务（service），服务之间消息通信
*/
class Server
{
public:
    void newService();
    void removeService();
    void sendToService();
    void sendToServer();
};