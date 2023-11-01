
#include <iostream>
#include "../third/lua/lua.hpp"
int main(int argc, char const *argv[])
{
    luaL_newstate();
    std::cout << "aa" << std::endl;
    return 0;
}
