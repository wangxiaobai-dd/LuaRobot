#include "worker.h"

void Worker::run()
{
    th = std::thread([this](){
        // io run
    });
}