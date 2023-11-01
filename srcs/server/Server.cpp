
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

void Server::runServer()
{
    this->setupServerConnections();
    this->acceptConnections();
}

void Server::setupServerConnections()
{
    for (size_t i = 0; i < http_servers.size(); ++i)
    {
        AcceptorSockets acceptor(http_servers[i].host, http_servers[i].port, http_servers[i].max_clients);
        acceptor.socketAPI();
        int socketFd = acceptor.getSocketFd();
        this->acceptorSockets.insert(std::pair<int, AcceptorSockets>(socketFd, acceptor));
    }
}

void Server::acceptConnections()
{
    std::map<int, AcceptorSockets>::iterator it = this->acceptorSockets.begin();
    std::map<int, AcceptorSockets>::iterator ite = this->acceptorSockets.end();

    while (true)
    {
        std::vector<pollfd> inputEventsContainer;
        // std::fill(fds.begin(), fds.end(), pollfd());

        size_t i = 0;
        for (; i < clientsFDs_Container.size(); i++)
        {
            pollfd tmp;
            tmp.fd = clientsFDs_Container[i];
            tmp.events = POLLIN;
            inputEventsContainer.push_back(tmp);
        }

        for (it = this->acceptorSockets.begin(); it != ite; ++it)
        {
            pollfd tmp;
            tmp.fd = it->first;
            tmp.events = POLLIN;
            inputEventsContainer.push_back(tmp);
            ++i;
        }

        errno = 0;
        int result = poll(inputEventsContainer.data(), inputEventsContainer.size(), -1);
        if (result == -1)
        {
            if (errno == EINTR)
                continue;
            throw std::runtime_error("poll() failed");
        }

          // Check for new client connections
        for (it = this->acceptorSockets.begin(); it != ite; ++it)
        {
            int serverFD = it->first;
            AcceptorSockets& acceptor = it->second;

            for (size_t k = 0; k < inputEventsContainer.size(); k++)
            {
                if (inputEventsContainer[k].fd == serverFD && inputEventsContainer[k].revents & POLLIN)
                {
                    int newClient = acceptor.accept_socket();
                    if (newClient == -1 || newClient == -503)
                        continue;
                    clientsFDs_Container.push_back(newClient);
                }
            }
        }

        std::cout << "fds.size() = " << inputEventsContainer.size() << std::endl;
        std::cout << "FDs_Container.size() = " << clientsFDs_Container.size() << std::endl;

        // Handle client connections
        for (size_t j = 0; j < inputEventsContainer.size() - acceptorSockets.size(); j++)
        {
            if (inputEventsContainer[j].revents & POLLIN)
            {
                read_socket(inputEventsContainer[j].fd);
            }
        }
    }
}

void Server::read_socket(int clientFD)
{
    char buffer[1024] = {0};
    int valread = read(clientFD, buffer, sizeof(buffer));
    if (valread == -1)
    {
        throw std::runtime_error("read() failed");
    }
    // read() returns 0 ==> client closed the connection. remove its FD
    if (valread == 0)
    {
        for (std::vector<int>::iterator it = this->clientsFDs_Container.begin(); it != this->clientsFDs_Container.end(); it++)
        {
            if (*it == clientFD)
            {
                this->clientsFDs_Container.erase(it);
                break;
            }
        }
        close(clientFD);
    }
    else
    {
        // Process the received message
        std::string message(buffer);
        std::cout << "Received message from client " << clientFD << ": " << message << std::endl;

        // Send a response back to the client
        std::string response = "Server received your message: " + message;
        write_socket(clientFD, response);
    }
}

void Server::write_socket(int clientFD, const std::string& message) {
    int valwrite = write(clientFD, message.c_str(), message.length());
    if (valwrite == -1) {
        throw std::runtime_error("write() failed");
    }
    std::cout << "Sent response to client " << clientFD << ": " << message << std::endl;
    std::string response = "hello from Server: \n";
    send(clientFD, response.c_str(), response.length(), 0);
}


void Server::printServerSettings()
{
    for (size_t i = 0; i < this->http_servers.size(); ++i)
    {
        std::cout << "=================Server[" << i + 1 << "]====================" << std::endl;

        std::cout << "port         : " << http_servers[i].port << std::endl;
        std::cout << "host         : " << inet_ntoa(http_servers[i].host) << std::endl;
        std::cout << "server       : " << http_servers[i].server_name << std::endl;
        std::cout << "root         : " << http_servers[i].root << std::endl;
        std::cout << "AUTOINDEX    : " << http_servers[i].autoindex << std::endl;
        std::cout << "upload_path  : " << http_servers[i].upload_path << std::endl;
        std::cout << "bodySize     : " << http_servers[i].client_max_body_size << std::endl;
        std::cout << "max_clients  : " << http_servers[i].max_clients << std::endl;

        for (size_t j = 0; j < http_servers[i].index_vec.size(); ++j)
            std::cout << "indexs[" << j << "] : " << http_servers[i].index_vec[j] << std::endl;


        for (size_t j = 0; j < http_servers[i].vec_of_locations.size(); ++j)
        {
            std::cout << "\n  ==LOCATION=== \n";
            std::map<std::string, std::string>::const_iterator map_iter;
            for (map_iter = http_servers[i].vec_of_locations[j].begin(); map_iter != http_servers[i].vec_of_locations[j].end(); ++map_iter)
            {
                std::cout << map_iter->first << " : " << map_iter->second << std::endl;
            }
        }

        std::cout << "\n  ==ERROR PAGE=== \n";
        std::map<int, std::string>::const_iterator map_iter;
        for (map_iter = http_servers[i].map_of_error_page.begin(); map_iter != http_servers[i].map_of_error_page.end(); ++map_iter)
        {
            std::cout << map_iter->first << " : " << map_iter->second << std::endl;
        }
    }
}
