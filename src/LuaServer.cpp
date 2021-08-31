#include "LuaServer.h"
#include <psp2/net/net.h>
#include <psp2/io/stat.h>
#include <lua.hpp>
#include <cstring>

LuaServer::LuaServer() : Server("Lua server", 1010, [](int socket)
{
    // Receive data
    char* buffer = new char[READ_BUFFER_SIZE];
    memset(buffer, '\0', READ_BUFFER_SIZE); // Null terminate just in case
    sceNetRecv(socket, buffer, READ_BUFFER_SIZE, 0);

    // Client-side JS forces us to use HTTP, so advance until the HTTP
    // header is finished (code begins after text "LUA ")
    char* luaBegin = buffer;
    while ((luaBegin[0] == 'L' && luaBegin[1] == 'U' && luaBegin[2] == 'A' && luaBegin[3] == ' ') == false)
    {
        luaBegin++;
        if (luaBegin >= buffer + READ_BUFFER_SIZE-5) return;
    }
    
    luaBegin += 4;

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
    if (luaL_dostring(L, luaBegin) != 0)
    {
        // Error occured; capture output
        usedBytes += snprintf(buffer+usedBytes, READ_BUFFER_SIZE-usedBytes, "%s\n", lua_tostring(L, -1));
    }
    buffer[usedBytes] = '\0';

    // JS makes us end a valid HTTP response for it to make any sense of it
    const std::string headers = "HTTP/1.1 200\nAccess-Control-Allow-Origin: *\n";
    const std::string contentLength = "Content-Length: " + std::to_string(usedBytes) + "\n\n";
    sceNetSend(socket, headers.c_str(), headers.length(), 0);
    sceNetSend(socket, contentLength.c_str(), contentLength.length(), 0);

    // Return output
    unsigned int bytes = (usedBytes < READ_BUFFER_SIZE) ? usedBytes : READ_BUFFER_SIZE;
    sceNetSend(socket, buffer, bytes, 0);
    delete[] buffer;
}) {}

LuaServer::~LuaServer() {}
