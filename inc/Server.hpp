#pragma once

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <fstream>
#include <mutex>
#include <string.h>
#include <fcntl.h>
#include <filesystem>

#define MAX_CLIENTS 3
#define LISTEN_PORT 4242

class Server
{
private:
    int _socket;
    struct sockaddr_in _addr;
    socklen_t _addrlen;
    std::vector<int> _clients;
    struct pollfd fds[MAX_CLIENTS + 1];

public:
    Server()
    {
     _addrlen = sizeof(_addr);
    }
    ~Server(void);

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
    int getSocket() const;
    struct sockaddr_in getSockAddr() const;
    socklen_t getAddrLen() const;
    std::vector<int> getClient() const;
    std::string getLogFile() const;
    const struct pollfd *getFds() const;
    pid_t getDaemonPid() const;
    size_t getClientsCount() const;
    void shiftArray(int *size);
};
