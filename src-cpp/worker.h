#pragma once

#include <memory>
#include <thread>

class Server;

class Worker
{
public:
    Worker(Server* s, uint32_t id){}

    void run();

private:
    std::shared_ptr<Server> server;
    std::thread th;
};