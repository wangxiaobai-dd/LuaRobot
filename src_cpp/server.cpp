
#include "server.h"

std::vector<std::string> Server::luaDirs;
std::string Server::envPath;

Server::Server(uint32_t threads)
{
    for(uint32_t i = 0; i < threads; ++i)
    {
        workerVec.emplace_back(std::make_unique<Worker>(this, i + 1));
    }

    for(auto& w : workerVec)
    {
        w->run();
    }
}

void Server::newService(std::unique_ptr<ServiceOption> option)
{
    auto worker = getWorkerByWorkerID(option->workerID);
    if(!worker)
    {
        worker = nextMinWorker();
        if(!worker)
            return;
    }
    worker->newService(std::move(option));
}

void Server::run()
{
    // while(true)
    // {
    // }
    wait();
}

void Server::wait()
{
    for(auto& w : workerVec)
    {
        w->wait();
    }
}

WorkerPtr Server::getWorkerByWorkerID(uint32_t workerID)
{
    if(!workerID || workerID > workerVec.size())
        return nullptr;
    return workerVec[workerID - 1];
}

WorkerPtr Server::getWorkerByServiceID(uint32_t serviceID)
{
    return nullptr;
}

WorkerPtr Server::nextMinWorker()
{
    if(workerVec.empty())
        return nullptr;
    uint32_t min = std::numeric_limits<uint32_t>::max();
    uint32_t minWorkerID = 0;
    for(const auto& w : workerVec)
    {
        uint32_t num = w->serviceNum.load(std::memory_order_acquire);
        if(w->shareThread.load(std::memory_order_acquire) && num < min)
        {
            min = num;
            minWorkerID = w->workerID;
        }
    }
    if(!minWorkerID)
        return nullptr;
    else
        return workerVec[minWorkerID - 1];
}

void Server::onTimer(uint32_t serviceID, uint32_t timerID)
{
}