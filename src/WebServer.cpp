#include "WebServer.h"
#include <psp2/net/net.h>
#include <psp2/io/stat.h>
#include <cstring>

WebServer::WebServer() : Server("Web server", 8080, [](int socket)
{
    // Receive data
    char* buffer = new char[READ_BUFFER_SIZE];
    sceNetRecv(socket, buffer, READ_BUFFER_SIZE, 0);

    // Get and null terminate URL; format is "GET [path] HTTP/1.1"
    size_t pathLength = 0;
    char* url = buffer + 4;
    while (*(url + pathLength) != ' ') ++pathLength;
    url[pathLength] = '\0';

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
    delete[] buffer;
}) {}

WebServer::~WebServer() {}
