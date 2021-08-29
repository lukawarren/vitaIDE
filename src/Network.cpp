#include <psp2/kernel/processmgr.h>
#include <psp2/net/netctl.h>
#include <psp2/sysmodule.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#include <psp2/net/net.h>
#include <cstring>
#include <string>

#include "Network.h"

#include <debugScreen.h>
#define printf psvDebugScreenPrintf

#define NETWORK_BUFFER_SIZE 1024*1024
#define READ_BUFFER_SIZE 1024*1024*1

void InitNetworking()
{
    // Load module
    sceSysmoduleLoadModule(SCE_SYSMODULE_NET);

    // Init network
    SceNetInitParam sInit;
    sInit.memory = new char[NETWORK_BUFFER_SIZE];
    sInit.size = NETWORK_BUFFER_SIZE;
    sInit.flags = 0;
    sceNetInit(&sInit);

    // Init netctl (for controlling the network)
    sceNetCtlInit();    

    // Get network details
    SceNetCtlInfo sInfo;
    sceNetCtlInetGetInfo(SCE_NETCTL_INFO_GET_IP_ADDRESS, &sInfo);
    printf("Networking initialised with IP address %s\n",sInfo.ip_address);
}

void TerminateNetworking()
{
    sceNetTerm();
    sceNetCtlTerm();
    printf("Networking terminated\n");
}

std::string GetIP()
{
    SceNetCtlInfo sInfo;
    sceNetCtlInetGetInfo(SCE_NETCTL_INFO_GET_IP_ADDRESS, &sInfo);
    return std::string(sInfo.ip_address);
}

WebServer::WebServer()
{
    // Make socket
    socket = sceNetSocket("vitaIDE_server_sock", SCE_NET_AF_INET, SCE_NET_SOCK_STREAM, 0);

    // Bind socket to address
    SceNetSockaddrIn sAddr;
    sAddr.sin_family = SCE_NET_AF_INET;
    sAddr.sin_addr.s_addr = sceNetHtonl(SCE_NET_INADDR_ANY); // Convert from host to network byte order
    sAddr.sin_port = sceNetHtons(8080);                      // Convert from host to network byte order
    int status = sceNetBind(socket, (SceNetSockaddr*)&sAddr, sizeof(sAddr));
    printf("[Web server] Bound socket to fd %d with status 0x%08X\n", socket, status);

    // Start listening
    sceNetListen(socket, 128); // 128 backlog

    // Server thread
    thread = sceKernelCreateThread("vitaIDE_server_thread", ServerThread, 0x10000100, 0x10000, 0, 0, NULL);
    sceKernelStartThread(thread, sizeof(this), this);
}

int WebServer::ServerThread(SceSize args, void* argp)
{
    WebServer* server = (WebServer*)argp;
    printf("[Web server] Started on port 8080\n");

    while(1)
    {
        // Get connection (if any)
        SceNetSockaddrIn sClientAddr;
        unsigned int addrSize = sizeof(SceNetSockaddrIn);
        
        int clientSocket = sceNetAccept(server->socket, (SceNetSockaddr*)&sClientAddr, &addrSize);
        if (clientSocket >= 0)
        {
            // Make unique(ish) thread name
            static int nConnections = 0;
            const std::string clientName = "vitaIDE_client_thread_" + std::to_string(nConnections++);

            // Create client thread
            sceKernelStartThread
            (
                sceKernelCreateThread(clientName.c_str(), ClientThread, 0x10000100, 0x10000, 0, 0, NULL),
                sizeof(clientSocket),
                &clientSocket
            );
        }

        // Socket closed; close thread
        else sceKernelExitDeleteThread(0);
    }

    return 0;
}

int WebServer::ClientThread(SceSize args, void* argp)
{
    const int* socket = (int*)argp;

    // Receive data
    char* buffer = new char[READ_BUFFER_SIZE];
    sceNetRecv(*socket, buffer, READ_BUFFER_SIZE, 0);
    
    buffer[READ_BUFFER_SIZE] = '\0';
    printf("[Web server] %s\n", buffer);

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
    sceNetSend(*socket, header.c_str(), header.length(), 0);

    if (fd >= 0)
    {
        // Get size for Content-Length
        SceIoStat stat;
        sceIoGetstatByFd(fd, &stat);
        const std::string contentLength = "Content-Length: " + std::to_string(stat.st_size) + "\n\n";
        sceNetSend(*socket, contentLength.c_str(), contentLength.length(), 0);

        // Read file and send
        unsigned int bytesRead = 0;
        while ((bytesRead = sceIoRead(fd, buffer, READ_BUFFER_SIZE)) > 0)
            sceNetSend(*socket, buffer, bytesRead, 0);
    }

    // File not found, 404
    else
    {
        const std::string content =
            "Content-Length: 4\n\n"
            "404\n";
        
        sceNetSend(*socket, content.c_str(), content.length(), 0);
    }

    sceIoClose(fd);

    // Free resources
    sceNetSocketClose(*socket);

    delete [] buffer;
    sceKernelExitDeleteThread(0);
    return 0;
}

WebServer::~WebServer()
{
    // (closing the socket terminates our server thread too)
    sceNetSocketClose(socket);
}