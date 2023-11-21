
#include "Server.hpp"
#include <cassert>
#include <cstddef>
#include <iostream>


#include <sys/event.h>
#include <vector>


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


    std::map<int, AcceptorSockets>::iterator it;
    std::map<int, AcceptorSockets>::iterator ite = this->acceptorSockets.end();

    for (it = this->acceptorSockets.begin(); it != ite; ++it)
    {
        std::cout << "ServerFD: " << it->first << " instance: " << &(it->second) << std::endl;
        std::cout << "Sever IP: " << it->second.getServerIpPort() << std::endl;
        std::cout << "--------------------------\n";
    }

    // this->pollEventHandler();
    this->kqueueEventHandler();
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

void Server::pollEventHandler()
{
    std::map<int, AcceptorSockets>::iterator it;
    std::map<int, AcceptorSockets>::iterator ite = this->acceptorSockets.end();

    std::map<int, AcceptorSockets*>::iterator it2;
    std::map<int, AcceptorSockets*>::iterator ite2 = this->clientsFDs_Container.end();

    while (true)
    {
        std::vector<pollfd> inputEventsContainer;

        for (it = this->acceptorSockets.begin(); it != ite; ++it)
        {
            pollfd tmp;
            tmp.fd = it->first;
            tmp.events = POLLIN;
            inputEventsContainer.push_back(tmp);
        }

        for (it2 = this->clientsFDs_Container.begin(); it2 != ite2; ++it2)
        {
            pollfd tmp;
            tmp.fd = it2->first;
            tmp.events = POLLIN;
            inputEventsContainer.push_back(tmp);
        }

        errno = 0;
        int result = poll(inputEventsContainer.data(), inputEventsContainer.size(), -1);
        if (result == -1)
        {
            if (errno == EINTR) // (interrupted system call)
                continue;
            throw std::runtime_error("poll() failed");
        }

        for (size_t k = 0; k < acceptorSockets.size(); k++)
        {
            if (inputEventsContainer[k].revents & POLLIN)
            {
                std::map<int, AcceptorSockets>::iterator it = this->acceptorSockets.begin();
                std::advance(it, k);
                std::cout << "***Accepting new client connection for server [" << it->second.getServerIpPort() << "]***" << std::endl;
                assert(inputEventsContainer[k].fd == it->first);// ***DEBUG***
                int newClient = it->second.accept_socket();
                if (newClient == -1 || newClient == -503)
                    continue;

                clientsFDs_Container.insert(std::pair<int, AcceptorSockets*>(newClient, &(it->second)));
                std::cout << std::endl;
            }
        }

        // Handle client connections
        for (size_t j = acceptorSockets.size(); j < inputEventsContainer.size(); j++)
        {
            if (inputEventsContainer[j].revents & POLLHUP)
            {
                std::cout << "Client " << inputEventsContainer[j].fd << " POLLHUP" << std::endl;
                clientDisconnected(inputEventsContainer[j].fd, NULL);
                continue;
            }
            else if (inputEventsContainer[j].revents & POLLIN)
            {
                read_socket(inputEventsContainer[j].fd, NULL);
            }
        }

        std::cout << "fds_Container --> ";
        printFdsContainer(inputEventsContainer);
        std::cout << "FDs_Container --> ";
        printMap(clientsFDs_Container);
    }
}

// udata : user-defined value passed through the kernel unchanged.
const uint64_t SERVER_UDATA = 1337;

void Server::kqueueEventHandler() {
    std::map<int, AcceptorSockets>::iterator it;
    std::map<int, AcceptorSockets>::iterator ite = this->acceptorSockets.end();

    int kq = kqueue();
    if (kq == -1) {
        throw std::runtime_error("kqueue() failed");
    }
    // Register acceptor sockets for reading events
    for (it = this->acceptorSockets.begin(); it != ite; ++it) {
        struct kevent serverEvent;
        EV_SET(&serverEvent, it->first, EVFILT_READ, EV_ADD, 0, 0, reinterpret_cast<void *>(SERVER_UDATA));
        eventList.push_back(serverEvent);
    }

    while (true) {
     std::vector<struct kevent> triggeredEvents(eventList.size());
        int eventCount = kevent(kq, eventList.data(), eventList.size(), triggeredEvents.data(), triggeredEvents.size(), NULL);
        if (eventCount == -1) {
            if (errno == EINTR)
                continue;
            throw std::runtime_error("kevent() failed in event loop");
        }
        for (int i = 0; i < eventCount; ++i) {
            struct kevent& event = triggeredEvents[i];

            // Event is for the server socket (new client connection)
            if (event.udata == reinterpret_cast<void*>(SERVER_UDATA)) {
                int acceptorSocketFD = event.ident;
                it = this->acceptorSockets.find(acceptorSocketFD);
                // std::cout << "***Accepting new client connection for server [" << it->second.getServerIpPort() << "]***" << std::endl;
                if (it == this->acceptorSockets.end()) {
                    throw std::runtime_error("acceptor socket not found");
                }
                if (clientsFDs_Container.size() >= 100)
                {
                    std::string responseMessage = "<h1>503 Service Unavailable</h1>";
                    std::cout << responseMessage << std::endl;
                    continue;
                }
                int newClient = it->second.accept_socket();
                if (newClient == -1 || newClient == -503) {
                    // Handle error
                    // std::cerr << "accept() failed" << std::endl;
                    continue;
                }
                clientsFDs_Container.insert(std::make_pair(newClient, &(it->second)));
                std::cout << "New client connected: " << newClient << std::endl;

                std::string responseMessage = "<h1>Hello client nbr " +
                                              std::to_string(newClient) +
                                              "</h1>";
                std::string httpRes = "HTTP/1.1 200 OK\r\n"
                                      "Content-Type: text/html\r\n"
                                      "Content-Length: " +
                                      std::to_string(responseMessage.length()) +
                                      "\r\n"
                                      "\r\n" +
                                      responseMessage;
                write_socket(newClient, httpRes);

                // Add new client socket to the event list
                struct kevent clientEvent;
                EV_SET(&clientEvent, newClient, EVFILT_READ, EV_ADD, 0, 0, reinterpret_cast<void*>(acceptorSocketFD));
                eventList.push_back(clientEvent);
            } else {
                // Event is for a client socket (data available to read)
                if (event.flags & EV_EOF) {
                    // Client closed the connection
                    std::cout << "Client " << event.ident << " *** disconnected ****" << std::endl;
                    clientDisconnected(event.ident, event.udata);
                    continue;
                }
                int clientFd = event.ident;
                std::cout << "Data available to read from client: " << clientFd << std::endl;
                read_socket(clientFd, event.udata);
            }
        }
    }
}

void Server::clientDisconnected(int clientFD, void *serverFd) {
    if (serverFd == NULL) // handle poll Event notifier mechanism
    {
        std::map<int, AcceptorSockets*>::iterator it = this->clientsFDs_Container.find(clientFD);
        if (it != this->clientsFDs_Container.end())
        {
            it->second->removeClient(clientFD);
            std::cout << "Client " << clientFD << " disconnected" << std::endl;
            this->clientsFDs_Container.erase(it);
        }
        close(clientFD);
    }
    else // handle kqueue Event notifier mechanism
    {
        std::vector<struct kevent>::iterator it = eventList.begin();
        std::vector<struct kevent>::iterator ite = eventList.end();
        while (it != ite) {
            if (static_cast<int>(it->ident) == clientFD) {
                eventList.erase(it);
                break;
            }
            ++it;
        }
        std::map<int, AcceptorSockets>::iterator iterServer =
            this->acceptorSockets.find(static_cast<int>(reinterpret_cast<intptr_t>(serverFd)));
        if (iterServer != this->acceptorSockets.end()) {
            iterServer->second.removeClient(clientFD);
            std::cout << "Client " << clientFD << " disconnected" << std::endl;
        }
        close(clientFD);
    }
}

void Server::read_socket(int clientFD, void* serverFd)
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
        clientDisconnected(clientFD, serverFd);
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
