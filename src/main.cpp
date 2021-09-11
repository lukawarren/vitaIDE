#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h> 

extern "C"
{
    #include <ftpvita.h>
}

#include "WebServer.h"
#include "LuaServer.h"

#include <debugScreen.h>
#define printf psvDebugScreenPrintf

// Silence FTP
static void LogFTP(const char*) {}

int main()
{
    // Disable sleep, setup printf's and load the networking module
    sceKernelPowerTick(SCE_KERNEL_POWER_TICK_DISABLE_AUTO_SUSPEND);
    psvDebugScreenInit();
    InitNetworking();

    // Init libftpvita
    ftpvita_set_info_log_cb(LogFTP);
    printf("[FTP Server] Started on port 1337\n");

    char ip[16];
    unsigned short int port;
    while (ftpvita_init(ip, &port) < 0) {}

    ftpvita_add_device("app0:");
	ftpvita_add_device("ux0:");

    // Start HTTP and Lua servers
    LuaServer* luaServer = new LuaServer;
    WebServer* webServer = new WebServer;
    sceKernelDelayThread(1000000);

    // Hang until input
    printf("\nPress CIRCLE to quit...\n");
    SceCtrlData ctrl = {};
    while ((ctrl.buttons & SCE_CTRL_CIRCLE) == false)
        sceCtrlPeekBufferPositive(0, &ctrl, 1);

    // Terminate servers
    delete webServer;
    delete luaServer;

    // Clean up
    ftpvita_fini();
    TerminateNetworking();
    sceKernelDelayThread(1000000);
    sceKernelExitProcess(0);
    return 0;
}