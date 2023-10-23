
#pragma once

#include "ParseConfigFile.hpp"
#include "AcceptorSockets.hpp"

class Server
{
private:
    std::vector<ServerSettings> http_servers;
    // std::vector<AcceptorSockets> acceptorSockets;
    std::map<int, AcceptorSockets> acceptorSockets;

public:
    Server();
    Server(std::vector<ServerSettings> http_servers);
    ~Server();
    void setupServerConnections();
    void acceptConnections();
    void runServer();
};