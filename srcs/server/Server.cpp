
#include "Server.hpp"

Server::Server()
{
}

Server::Server(std::vector<ServerSettings> http_servers)
{
    this->http_servers = http_servers;
}

Server::~Server()
{
}

void Server::setupServerConnections()
{
    for (size_t i = 0; i < http_servers.size(); ++i)
    {
        AcceptorSockets acceptor(this->http_servers[i].host, this->http_servers[i].port);
        acceptor.create_socket();
        acceptor.bind_socket();
        acceptor.listen_socket();
        int socketFd = acceptor.getSocketFd();
        this->acceptorSockets.insert(std::pair<int, AcceptorSockets>(socketFd, acceptor));
    }
}

void Server::runServer()
{
    this->setupServerConnections();
    this->runServer();
}

void Server::acceptConnections()
{
    while (true)
    {
        std::map<int, AcceptorSockets>::iterator it = this->acceptorSockets.begin();
        std::map<int, AcceptorSockets>::iterator ite = this->acceptorSockets.end();
        for (; it != ite; ++it)
        {
            int socketFd = it->first;
            AcceptorSockets& acceptor = it->second;
            if (acceptor.checkMaxClients())
            {
                acceptor.accept_socket();
                acceptor.run(); // Run the AcceptorSockets logic for the new client connection
            }
        }
    }
}
