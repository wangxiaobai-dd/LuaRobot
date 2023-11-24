#pragma once

#include <atomic>
#include <memory>
#include <thread>
#include <unordered_map>
#include "common.h"
#include "service.h"

class Server;

class Worker
{
public:
    Worker() = delete;
    Worker(Server* s, uint32_t _workerID);

    void run();
    void stop();
    void newService(std::unique_ptr<ServiceOption> option);
    void send();
    void wait();

    std::atomic_uint32_t serviceNum = 0;
    std::atomic_bool shareThread = true;
    uint32_t workerID = 0;

private:
    Server* server = nullptr;
    std::thread th;
    std::unordered_map<uint32_t, std::unique_ptr<LuaService>> serviceMap;
    uint32_t genID = 0;
};

using WorkerPtr = std::shared_ptr<Worker>;