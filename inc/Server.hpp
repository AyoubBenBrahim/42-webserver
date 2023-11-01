
// #pragma once

// #include "ParseConfigFile.hpp"
// #include "AcceptorSockets.hpp"

// class Server
// {
// private:
//     std::vector<ServerSettings> http_servers;
//     std::map<int, AcceptorSockets> acceptorSockets;

//     std::vector<int> _clientsFD;
//     // int clientsCount;
//     // struct pollfd fds[100 + 1];

// public:
//     Server();
//     Server(std::vector<ServerSettings> http_servers);
//     ~Server();
//     void setupServerConnections();
//     void acceptConnections();
//     void runServer();
//     void printServerSettings();
//     void read_socket(int client);
//     void write_socket(int client, const std::string& message);
// };



#pragma once

#include "ParseConfigFile.hpp"
#include "AcceptorSockets.hpp"

class Server
{
private:
    std::vector<ServerSettings> http_servers;
    std::map<int, AcceptorSockets> acceptorSockets;

    std::vector<int> clientsFDs_Container;

public:
    Server();
    Server(std::vector<ServerSettings> http_servers);
    ~Server();
    void setupServerConnections();
    void acceptConnections();
    void runServer();
    void printServerSettings();
    void read_socket(int client);
    void write_socket(int client, const std::string& message);
};
