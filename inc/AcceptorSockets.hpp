#pragma once

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <poll.h>
#include <fstream>
#include <mutex>
#include <string.h>
#include <fcntl.h>
#include <filesystem>

class AcceptorSockets
{
private:
    int                 _AcceptorSocketFd;
    int                 listen_port;
    in_addr             host;
    size_t              backlogQueueMax; // max clients
    struct sockaddr_in  _addr;
    socklen_t           _addrlen;
    std::vector<int>    _clientsFD;

public:
    AcceptorSockets(in_addr host, int port, int max_clients);
    // AcceptorSockets(const AcceptorSockets &src) = delete;
    // AcceptorSockets &operator=(const AcceptorSockets &src) = delete;
    ~AcceptorSockets(void);

    void                socketAPI(void);
    void                create_socket(void);
    void                bind_socket(void);
    void                listen_socket(void);
    int                 accept_socket(void);
    bool                checkMaxClients(void);
    int                 getSocketFd() const;
    struct sockaddr_in  getSockAddr() const;
    socklen_t           getAddrLen() const;
    void                removeClient(int client);

    void printClientsFDs(void)
    {
        std::cout << "size: " << this->_clientsFD.size() << " [ ";
        for (size_t i = 0; i < this->_clientsFD.size(); i++)
        {
            std::cout << this->_clientsFD[i] << " ";
        }
        std::cout << "]" << "\n" << std::endl;
    }
};
