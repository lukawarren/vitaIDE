#include <psp2/kernel/processmgr.h>

#include "lua.hpp"
#include "debugScreen.h"
#define printf psvDebugScreenPrintf

#include "Network.h"

// Lua stdout callback
static int OnLuaPrint(lua_State* L)
{
    int nArgs = lua_gettop(L);

    for (int i = 1; i <= nArgs; ++i)
    {
        printf("%s\n", luaL_tolstring(L, i, nullptr));
    }

    return 0;
}

int main()
{
    psvDebugScreenInit();

    InitNetworking();

    Server server;
    
    while(1){}

    /*
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    lua_pushcfunction(L, OnLuaPrint);
    lua_setglobal(L, "print");
    luaL_dostring(L, "print(\"Hello from Lua! 2+2 is \" .. tostring(2+2))");
    printf("Hello from C!\n");
    lua_close(L);
    */

    TerminateNetworking();
    sceKernelDelayThread(3*1000000);
    sceKernelExitProcess(0);
    return 0;
}