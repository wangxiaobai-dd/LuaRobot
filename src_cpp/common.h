#pragma once

#include "lua.hpp"
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>

constexpr uint32_t WORKER_ID_SHIFT = 24;

struct CallbackContext
{
    lua_State* callbackL = nullptr;
};

// 服务配置
struct ServiceOption
{
    uint32_t workerID = 0;
    std::string luaFile;
    std::string envPath; // lua 环境变量 搜索模块
};

struct StateDeleter
{
    void operator()(lua_State* L) const
    {
        lua_close(L);
    }
};

// test
template <typename T>
class ServiceQueue
{
public:
    void push(T&& msg)
    {
        std::lock_guard<std::mutex> lock(mtx);
        msgQueue.push(msg);
    }
    T pop()
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto msg = msgQueue.front();
        return msg;
    }

private:
    std::queue<T> msgQueue;
    std::mutex mtx;
};
