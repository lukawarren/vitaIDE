#include <psp2/kernel/processmgr.h>

#include <lua.hpp>
#include <debugScreen.h>
#define printf psvDebugScreenPrintf

extern "C"
{
    #include <ftpvita.h>
}

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

static void LogFTP(const char* s)
{
    printf("[libftpvita] %s", s);
}

int main()
{
    sceKernelPowerTick(SCE_KERNEL_POWER_TICK_DISABLE_AUTO_SUSPEND);
    psvDebugScreenInit();
    InitNetworking();

    // Init libftpvita
    ftpvita_set_info_log_cb(LogFTP);
    printf("Starting FTP on port 1337....\n");

    char ip[16];
    unsigned short int port;
    while (ftpvita_init(ip, &port) < 0) {}

    ftpvita_add_device("app0:");
	ftpvita_add_device("ux0:");

    // Start web server
    WebServer server;
    
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

    ftpvita_fini();
    TerminateNetworking();
    sceKernelDelayThread(3*1000000);
    sceKernelExitProcess(0);
    return 0;
}