#include "worker.h"
#include "server.h"

Worker::Worker(Server* s, uint32_t _workerID) : server(s), workerID(_workerID), ctxGuard(asio::make_work_guard(ioCtx))
{
}

Worker::~Worker()
{
}

void Worker::run()
{
    th = std::thread([this]()
                     {
                         ioCtx.run();

                         // io run queue
                     });
}

void Worker::stop()
{
}

void Worker::wait()
{
    ioCtx.stop();

    if(th.joinable())
        th.join();
}

void Worker::newService(std::unique_ptr<ServiceOption> option)
{
    // push func into queue
    serviceNum.fetch_add(1, std::memory_order_release);
    // todo thread safe

    asio::post(ioCtx, [this, option = std::move(option)]()
               {
        do
        {
            uint32_t id = ++genID | (workerID << WORKER_ID_SHIFT);
            auto service = std::make_unique<LuaService>();
            if(!service)
                break;
            service->setContext(server, this);
            service->init(*option);
            serviceMap.emplace(id, std::move(service));
            return;
        } while(false);

        serviceNum.fetch_sub(1, std::memory_order_release); });
}

/*
void worker::send(message&& msg)
{
    ++mqsize_;
    if(mq_.push_back(std::move(msg)) == 1)
    {
        asio::post(io_ctx_, [this]()
                   {
                if (!mq_.try_swap(swapmq_))
                    return;

                service* s = nullptr;
                for (auto& msg : swapmq_)
                {
                    handle_one(s, std::move(msg));
                    --mqsize_;
                }

                swapmq_.clear(); });
    }
}
*/