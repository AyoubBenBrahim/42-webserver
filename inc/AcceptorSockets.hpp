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

// #define MAX_CLIENTS 3
// #define LISTEN_PORT 4242

class AcceptorSockets
{
private:
    int _socketFd;
    int listen_port;
    in_addr host;
    int backlogQueueMax; // max clients in queue
    struct sockaddr_in _addr;
    socklen_t _addrlen;
    std::vector<int> _clientsFD;
    struct pollfd fds[backlogQueueMax + 1];

public:
    // AcceptorSockets()
    // {
    //  _addrlen = sizeof(_addr);
    // }
    AcceptorSockets(in_addr host, int port);
    ~AcceptorSockets(void);

    void create_socket(void);
    void bind_socket(void);
    void listen_socket(void);
    bool accept_socket(void);
    void read_socket(int client, int *clientCount);
    void write_socket(int client, const std::string& message);
    // void write_socket(int client);
    void close_socket(void);
    bool checkMaxClients(void);
    void run(void);
    int getSocketFd() const;
    struct sockaddr_in getSockAddr() const;
    socklen_t getAddrLen() const;
    std::vector<int> getClient() const;
    std::string getLogFile() const;
    const struct pollfd *getFds() const;
    size_t getClientsCount() const;
    void shiftArray(int *size);
};
