#pragma once

#include "service.h"
#include "worker.h"
#include <unordered_map>
#include <vector>

/*
    actor 模型，每个线程 一或多 服务（service），服务之间消息通信
*/
class Server : public std::enable_shared_from_this<Server>
{
public:
    class TimerExpire
    {
    public:
        TimerExpire() = default;

        TimerExpire(uint32_t _timerID, uint32_t _serviceID, Server* srv)
            : timerID(_timerID), serviceID(_serviceID), server(srv) {}

        void operator()()
        {
            server->onTimer(serviceID, timerID);
        }

        uint32_t id() const
        {
            return timerID;
        }

    private:
        uint32_t timerID = 0;
        uint32_t serviceID = 0;
        Server* server = nullptr;
    };

    Server() = delete;
    Server(uint32_t threads = 1);
    void newService(std::unique_ptr<ServiceOption> option);
    void removeService();
    void sendToService();
    void sendToServer();
    void run();
    void onTimer(uint32_t serviceID, uint32_t timerID);

    WorkerPtr getWorkerByWorkerID(uint32_t workerID);
    WorkerPtr getWorkerByServiceID(uint32_t serviceID);
    WorkerPtr nextMinWorker();

    static std::string envPath;
    static std::vector<std::string> luaDirs;

private:
    void wait();
    std::vector<WorkerPtr> workerVec;
};