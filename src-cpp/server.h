#pragma once

#include "service.h"
#include "service_type.h"
#include "worker.h"
#include <unordered_map>
#include <vector>

/*
    actor ģ�ͣ�ÿ���߳� һ��� ����service��������֮����Ϣͨ��
*/
class Server : public std::enable_shared_from_this<Server>
{
public:
    Server() = delete;
    Server(uint32_t threads = 1);
    void newService();
    void removeService();
    void sendToService();
    void sendToServer();
    void run();

private:
    std::vector<std::unique_ptr<Worker>> workerVec;
    std::unordered_map<ServiceType, std::unique_ptr<LuaService>> serviceMap;
};