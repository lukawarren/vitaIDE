#include <psp2/kernel/processmgr.h>
#include <psp2/io/stat.h>
#include <cstring>
#include <string>

#include <lua.hpp>
#include <debugScreen.h>
#define printf psvDebugScreenPrintf

extern "C"
{
    #include <ftpvita.h>
}

#include "Network.h"

static void LogFTP(const char* s)
{
    printf("[libftpvita] %s", s);
}

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

    // Start web server
    Server webServer("Web server", 8080, [](int socket)
    {
        // Receive data
        char* buffer = new char[READ_BUFFER_SIZE];
        sceNetRecv(socket, buffer, READ_BUFFER_SIZE, 0);

        // Get and null terminate URL; format is "GET [path] HTTP/1.1"
        size_t pathLength = 0;
        char* url = buffer + 4;
        while (*(url + pathLength) != ' ') ++pathLength;
        url[pathLength] = '\0';
        printf("[Web server] HTTP GET %s\n", url);

        // Open file
        std::string path = std::string("ux0:/data/vitaIDE/site/dist") + url;
        if (strcmp(url, "/") == 0) path += "index.html";
        auto fd = sceIoOpen
        (
            path.c_str(),
            SCE_O_RDONLY,
            0777
        );

        // HTTP reponse header
        const std::string status = (fd >= 0) ? "200" : "404";
        const std::string header = "HTTP/1.1 " + status + " OK\n";
        sceNetSend(socket, header.c_str(), header.length(), 0);

        if (fd >= 0)
        {
            // Get size for Content-Length
            SceIoStat stat;
            sceIoGetstatByFd(fd, &stat);
            const std::string contentLength = "Content-Length: " + std::to_string(stat.st_size) + "\n\n";
            sceNetSend(socket, contentLength.c_str(), contentLength.length(), 0);

            // Read file and send
            unsigned int bytesRead = 0;
            while ((bytesRead = sceIoRead(fd, buffer, READ_BUFFER_SIZE)) > 0)
                sceNetSend(socket, buffer, bytesRead, 0);
        }

        // File not found, 404
        else
        {
            const std::string content =
                "Content-Length: 4\n\n"
                "404\n";
            
            sceNetSend(socket, content.c_str(), content.length(), 0);
        }

        sceIoClose(fd);
    });
    
    // Start Lua server
    Server luaServer("Lua server", 1010, [](int socket)
    {
        // Receive data
        char* buffer = new char[READ_BUFFER_SIZE];
        memset(buffer, '\0', READ_BUFFER_SIZE); // Null terminate just in case
        sceNetRecv(socket, buffer, READ_BUFFER_SIZE, 0);
        printf("[Lua server] %s\n", buffer);

        // Make new Lua state
        lua_State *L = luaL_newstate();
        luaL_openlibs(L);

        // Capture stdout into buffer
        unsigned int usedBytes = 0;
        lua_pushlightuserdata(L, buffer);
        lua_pushlightuserdata(L, &usedBytes);
        lua_pushcclosure(L, [](lua_State* L)
        {
            char* outputBuffer = (char*) lua_topointer(L, lua_upvalueindex(1));
            unsigned int* nBytes = (unsigned int*) lua_topointer(L, lua_upvalueindex(2));

            int nArgs = lua_gettop(L);

            for (int i = 1; i <= nArgs; ++i)
                *nBytes += snprintf(outputBuffer+*nBytes, READ_BUFFER_SIZE-*nBytes, "%s\n", luaL_tolstring(L, i, nullptr));
            
            return 0;
        }, 2);
        lua_setglobal(L, "print");

        // Run code and freshly null terminate (we can't clear the
        // buffer, so terminate after where it was last overwritten)
        if (luaL_dostring(L, buffer) != 0)
        {
            // Error occured; capture output
            usedBytes += snprintf(buffer+usedBytes, READ_BUFFER_SIZE-usedBytes, "%s\n", lua_tostring(L, -1));
        }
        buffer[usedBytes] = '\0';

        // Return output
        unsigned int bytes = (usedBytes < READ_BUFFER_SIZE) ? usedBytes : READ_BUFFER_SIZE;
        sceNetSend(socket, buffer, bytes, 0);

    }, false);

    ftpvita_fini();
    TerminateNetworking();
    sceKernelDelayThread(3*1000000);
    sceKernelExitProcess(0);
    return 0;
}