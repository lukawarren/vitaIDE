#pragma once
#include <psp2/types.h>
#include <functional>
#include <string>

#define NETWORK_BUFFER_SIZE 1024*1024
#define READ_BUFFER_SIZE 1024*1024

void InitNetworking();
void TerminateNetworking();

class Server
{
public:
    Server(const std::string& serverName, const unsigned short port, const std::function<void(int)> clientFunction);
    virtual ~Server();

    static int ServerThread(SceSize args, void* argp);
    static int ClientThread(SceSize args, void* argp);

    void Terminate();

private:
    int socket;
    unsigned int port;
    std::string serverName;
    std::function<void(int)> clientFunction;

    struct ClientThreadInfo
    {
        Server* server;
        int socket;
    };

};
