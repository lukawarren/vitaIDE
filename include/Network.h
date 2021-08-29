#pragma once

void InitNetworking();
void TerminateNetworking();

class WebServer
{
public:
    WebServer();
    ~WebServer();

    static int ServerThread(SceSize args, void* argp);
    static int ClientThread(SceSize args, void* argp);

private:
    int socket;
    SceUID thread;
};
