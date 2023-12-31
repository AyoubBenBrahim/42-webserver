#pragma once

#include "ParseConfigFile.hpp"
#include "AcceptorSockets.hpp"

class Server
{
private:
    std::vector<ServerSettings> http_servers;
    std::map<int, AcceptorSockets> acceptorSockets;
    std::map<int, AcceptorSockets*> clientsFDs_Container;

    std::vector<struct kevent> eventList;

public:
    Server();
    Server(std::vector<ServerSettings> http_servers);
    ~Server();
    void setupServerConnections();
    void runServer();
    void printServerSettings();
    // void read_socket(int clientFD);
    void read_socket(int clientFD, void *serverFd);
    void write_socket(int clientFD, const std::string& message);

    // void acceptConnections();
    // void acceptConnectionskqueue();
    void pollEventHandler();
    void kqueueEventHandler();

    void clientDisconnected(int clientFD, void* serverFd);

    void printFdsContainer(std::vector<pollfd> fdsContainer)
    {
        std::vector<pollfd>::iterator it;
        std::vector<pollfd>::iterator ite = fdsContainer.end();

        std::cout << "size: " << fdsContainer.size() << " [ ";
        for (it = fdsContainer.begin(); it != ite; ++it)
        {
            std::cout << it->fd << " ";
        }
        std::cout << "]" << std::endl;
    }

    void printMap(const std::map<int, AcceptorSockets*>& map)
{
    std::map<int, AcceptorSockets*>::const_iterator it;
    std::map<int, AcceptorSockets*>::const_iterator ite = map.end();

    std::cout << "size: " << map.size() << " [ ";
    for (it = map.begin(); it != ite; ++it)
    {
        std::cout << it->first << " ";
    }
    std::cout << "]" << std::endl;
}

void printMap(const std::map<int, AcceptorSockets>& map)
{
    std::map<int, AcceptorSockets>::const_iterator it;
    std::map<int, AcceptorSockets>::const_iterator ite = map.end();

    std::cout << "size: " << map.size() << " [ ";
    for (it = map.begin(); it != ite; ++it)
    {
        std::cout << it->first << " ";
    }
    std::cout << "]" << std::endl;
}
};
