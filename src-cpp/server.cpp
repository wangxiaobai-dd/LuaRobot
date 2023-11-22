
#include "server.h"

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

void Server::newService(uint32_t workerID)
{
    auto worker = getWorker(workerID);
    if(!worker)
    {
        worker = nextWorker();
        if(!worker)
            return;
    }
    worker->newService();
}

void Server::run()
{
    
}

WorkerPtr Server::getWorker(uint32_t workerID)
{
    if(!workerID || workerID > workerVec.size())
        return nullptr;
    return workerVec[workerID - 1];
}

WorkerPtr Server::nextWorker()
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