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
    Server* server;
    std::thread th;
};