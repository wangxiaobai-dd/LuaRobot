#include "worker.h"
#include "server.h"

Worker::Worker(Server* s, uint32_t _workerID) : server(s), workerID(_workerID)
{
}

void Worker::run()
{
    th = std::thread([this]()
                     {
                         // io run queue
                     });
}

void Worker::stop()
{
}

void Worker::newService()
{
    // push func into queue
    serviceNum.fetch_add(1, std::memory_order_release);
    auto func = [this]()
    {
        do
        {
            uint32_t id = ++genID | (workerID << WORKER_ID_SHIFT);
            auto service = std::make_unique<LuaService>();
            if(!service)
                break;
            service->setContext(server, this);
            serviceMap.emplace(id, std::move(service));
            return;
        } while(false);

        serviceNum.fetch_sub(1, std::memory_order_release);
    };
}

void Worker::send()
{
    
}
