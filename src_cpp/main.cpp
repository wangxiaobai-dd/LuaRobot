
#include "lua_utility.h"
#include "server.h"
#include "service.h"
#include <filesystem>
#include <iostream>
#include <thread>

static void usage()
{
    std::cout << "��ָ���ű� xxx.lua, ./main example.lua" << std::endl;
}

static void report(std::string_view what)
{
    std::cerr << what << std::endl;
}

int main(int argc, char const* argv[])
{
    if(argc < 1)
    {
        usage();
        return -1;
    }

    // std::string script = argv[1];
    std::string script = "../lua/example.lua";
    if(std::filesystem::path(script).extension() != ".lua")
    {
        usage();
        return -1;
    }

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    int err = luaL_loadfile(L, script.data());
    if(err != LUA_OK)
    {
        report("load lua fail");
        return -1;
    }

    err = lua_pcall(L, 0, 1, 0);
    if(err != LUA_OK)
    {
        report("exec lua fail");
        return -1;
    }

    if(lua_type(L, -1) != LUA_TTABLE)
    {
        report("need load config table");
        return -1;
    }
    uint32_t threads = lua_opt_field<uint32_t>(L, -1, "threads");
    std::cout << "threads:" << threads << std::endl;

    // package path
    std::string path = lua_opt_field<std::string>(L, -1, "path");
    std::cout << "path:" << path << std::endl;

    auto server = std::make_shared<Server>(threads);
    server->envPath = path;

    // test
    auto option = std::make_unique<ServiceOption>();
    option->luaFile = "../lua-test/testservice.lua";
    server->newService(std::move(option));

    server->run();

    return 0;
}
