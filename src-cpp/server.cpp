
#include "server.h"

Server::Server(uint32_t threads)
{
    for(uint32_t i = 0; i < threads; ++i)
    {
       workerVec.emplace_back(std::make_unique<Worker>(shared_from_this(), i + 1));
    }

    for(auto& w : workerVec)
    {
        w->run();
    }
}

void Server::newService()
{
}

void Server::run()
{
}