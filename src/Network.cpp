#include <psp2/kernel/processmgr.h>
#include <psp2/net/netctl.h>
#include <psp2/sysmodule.h>
#include <psp2/io/fcntl.h>
#include <psp2/net/net.h>
#include <string>

#include "Network.h"

#include <debugScreen.h>
#define printf psvDebugScreenPrintf

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
    printf("Networking initialised with IP address %s\n", sInfo.ip_address);
}

void TerminateNetworking()
{
    sceNetTerm();
    sceNetCtlTerm();
    printf("Networking terminated\n");
}

Server::Server(const std::string& serverName, const unsigned short port, const std::function<void(int)> clientFunction) :
    port(port), serverName(serverName), clientFunction(clientFunction)
{
    // Make socket
    socket = sceNetSocket("vitaIDE_server_sock", SCE_NET_AF_INET, SCE_NET_SOCK_STREAM, 0);

    // Bind socket to address
    SceNetSockaddrIn sAddr;
    sAddr.sin_family = SCE_NET_AF_INET;
    sAddr.sin_addr.s_addr = sceNetHtonl(SCE_NET_INADDR_ANY); // Convert from host to network byte order
    sAddr.sin_port = sceNetHtons(port);                      // Convert from host to network byte order
    int status = sceNetBind(socket, (SceNetSockaddr*)&sAddr, sizeof(sAddr));

    if (status != 0)
        printf("[%s] Unexpected status %d when attempting to bind port %d to fd %d\n",
            serverName.c_str(), status, port, socket);

    // Start listening
    sceNetListen(socket, 128); // 128 backlog

    // Server thread
    Server* argp = this;
    std::string threadName = "vitaIDE_server_thread_" + std::to_string(port);
    auto thread = sceKernelCreateThread(threadName.c_str(), ServerThread, 0x10000100, 0x10000, 0, 0, NULL);
    sceKernelStartThread(thread, sizeof(argp), &argp);
}

int Server::ServerThread(SceSize args, void* argp)
{
    Server* server = *(Server**)argp;
    printf("[%s] Started on port %d\n", server->serverName.c_str(), server->port);

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
            const std::string clientName =
                "vitaIDE_port_" + std::to_string(server->port) + "_client_" + std::to_string(nConnections++);

            // Make argp
            ClientThreadInfo argp;
            argp.server = server;
            argp.socket = clientSocket;

            // Create client thread
            sceKernelStartThread
            (
                sceKernelCreateThread(clientName.c_str(), ClientThread, 0x10000100, 0x10000, 0, 0, NULL),
                sizeof(argp),
                &argp
            );
        }

        // Socket closed; close thread
        else sceKernelExitDeleteThread(0);
    }

    return 0;
}

int Server::ClientThread(SceSize args, void* argp)
{
    // Call lambda passed in constructor
    ClientThreadInfo* info = (ClientThreadInfo*)argp;
    info->server->clientFunction(info->socket);

    // Cleanup
    sceNetSocketClose(info->socket);
    sceKernelExitDeleteThread(0);
    return 0;
}

Server::~Server()
{
    // (closing the socket terminates our server thread too)
    sceNetSocketClose(socket);
}