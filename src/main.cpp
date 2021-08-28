#include <psp2/kernel/processmgr.h>
#include <psp2/sysmodule.h>
#include <psp2/net/netctl.h>
#include <psp2/net/net.h> 

#include "debugScreen.h"
#define printf psvDebugScreenPrintf

#include "lua.hpp"

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

constexpr auto networkBufferSize = 1024*1024;
void InitNetwork();
void StartServer();
void TermNetwork();

int main()
{
    psvDebugScreenInit();
    
    InitNetwork();
    StartServer();
    TermNetwork();

    /*
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    lua_pushcfunction(L, OnLuaPrint);
    lua_setglobal(L, "print");
    luaL_dostring(L, "print(\"Hello from Lua! 2+2 is \" .. tostring(2+2))");
    printf("Hello from C!\n");
    lua_close(L);
    */

    sceKernelDelayThread(3*1000000);
    sceKernelExitProcess(0);
    return 0;
}

void InitNetwork()
{
    // Load module
    printf("Loading module SCE_SYSMODULE_NET\n");
    sceSysmoduleLoadModule(SCE_SYSMODULE_NET);

    printf("Calling SceNetInit\n");

    // Init network
    SceNetInitParam sInit;
    sInit.memory = new char[networkBufferSize];
    sInit.size = networkBufferSize;
    sInit.flags = 0;
    sceNetInit(&sInit);

    // Init netctl (for controlling the network)
    printf("Calling SceNetCtlInit\n");
    sceNetCtlInit();

    // Get network details
    SceNetCtlInfo sInfo;
    sceNetCtlInetGetInfo(SCE_NETCTL_INFO_GET_IP_ADDRESS, &sInfo);
    printf("IP address: %s\n", sInfo.ip_address);
}

void StartServer()
{
    // Make socket
    printf("Making socket...\n");
    int socket = sceNetSocket("vitaIDE_server_sock", SCE_NET_AF_INET, SCE_NET_SOCK_STREAM, 0);

    // Bind socket to address
    SceNetSockaddrIn sAddr;
    sAddr.sin_family = SCE_NET_AF_INET;
    sAddr.sin_addr.s_addr = sceNetHtonl(SCE_NET_INADDR_ANY); // Convert from host to network byte order
    sAddr.sin_port = sceNetHtons(1337);                      // Convert from host to network byte order
    int status = sceNetBind(socket, (SceNetSockaddr*)&sAddr, sizeof(sAddr));
    printf("Bound socket to fd %d with status 0x%08X\n", socket, status);

    // Start listening
    sceNetListen(socket, 128); // 128 backlog
    printf("Listening...\n");

    while (1)
    {
        // Get connection (if any)
        SceNetSockaddrIn sClientAddr;
        unsigned int addrSize = sizeof(SceNetSockaddrIn);
        int clientSocket = sceNetAccept(socket, (SceNetSockaddr*)&sClientAddr, &addrSize);

        if (clientSocket >= 0)
        {
            printf("Connection received at address ");

            // Get IP
            char ip[16];
            sceNetInetNtop(SCE_NET_AF_INET, &sClientAddr.sin_addr.s_addr, ip, sizeof(ip));
            printf("%s!\n", ip);

            // Say goodbye
            sceNetSocketClose(clientSocket);
        }
    }

    sceNetSocketClose(socket);
}

void TermNetwork()
{
    printf("Terminating network... ");
    sceNetTerm();
    sceNetCtlTerm();
    printf("done\n");
}
