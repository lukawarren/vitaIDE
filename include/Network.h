#pragma once

void InitNetworking();
void TerminateNetworking();

class Server
{
public:
    Server();
    ~Server();

    static int ServerThread(SceSize args, void* argp);
    static int ClientThread(SceSize args, void* argp);

private:
    int socket;
    SceUID thread;
};
