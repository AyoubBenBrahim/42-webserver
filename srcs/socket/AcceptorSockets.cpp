#include "AcceptorSockets.hpp"
#include <iostream>
#include <iterator>
#include <string>

AcceptorSockets::AcceptorSockets(in_addr host, int port, int max_clients)
{
    this->listen_port = port;
    this->host = host;
    this->backlogQueueMax = (max_clients == 0) ? 100 : max_clients;
    this->_addrlen = sizeof(_addr);
}

AcceptorSockets::~AcceptorSockets()
{
}

/*
    in our case:
    The poll function allows us to monitor multiple file descriptors for events
    and provides a way to handle them without blocking.
*/

void AcceptorSockets::socketAPI()
{ 
    this->create_socket();
    this->setSocketAddress();
    this->setSocketReuseAddr();
    this->bind_socket();
    this->setSocketNonBlocking();
    this->listen_socket(); // passive socket aka listening for incoming connections
}

int AcceptorSockets::getSocketFd() const
{
    return this->_AcceptorSocketFd;
}

struct sockaddr_in AcceptorSockets::getSockAddr() const
{
    return this->_addr;
}

socklen_t AcceptorSockets::getAddrLen() const
{
    return this->_addrlen;
}

std::string AcceptorSockets::getServerIpPort() const
{
    char ip[INET_ADDRSTRLEN];
    unsigned short port;

    inet_ntop(AF_INET, &(this->_addr.sin_addr), ip, INET_ADDRSTRLEN);
    port = ntohs(this->_addr.sin_port);

    std::string ip_port = ip;
    ip_port += ":";
    ip_port += std::to_string(port);
    return ip_port;
}

void AcceptorSockets::create_socket()
{
    // fd = socket(domain, type, protocol)
    _AcceptorSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_AcceptorSocketFd == -1)
    {
        throw std::runtime_error("socket() failed");
    }
}

/*
    _addr.sin_addr.s_addr = inet_addr(host.c_str());
    assigns the IP address specified by the host variable

    INADDR_ANY represents the IP address "0.0.0.0",
    indicating that the server socket should bind and
    listen on all available network interfaces on the system.
    This allows the server to accept incoming connections
    on any IP address associated with the machine.
*/
void AcceptorSockets::setSocketAddress() {

    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(this->listen_port); // htons = host to network short
    _addr.sin_addr.s_addr = host.s_addr;
    // _addr.sin_addr.s_addr = INADDR_ANY;
    // memcpy(&_addr.sin_addr, &host, sizeof(host));
}

/*
setsockopt(): used to set options on a socket.
    allows to customize various socket parameters. 

SO_REUSEADDR: This option indicates that the local address can be reused.
    It allows binding to a local address and port that is in a TIME_WAIT state.
    This option is commonly used to avoid the "Address already in use" error
    when binding to a recently closed socket.
*/

void AcceptorSockets::setSocketReuseAddr()
{
    int reuseAddr = 1;
    if (setsockopt(_AcceptorSocketFd, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr)) == -1)
    {
        throw std::runtime_error("setsockopt() failed");
    }
}

/*

fcntl(): = file control : used to manipulate the file descriptor flags of a socket.
    fcntl() is used to set the O_NONBLOCK flag, which makes the socket non-blocking.
    Non-blocking sockets allow for asynchronous I/O operations,
    enabling more efficient handling of multiple connections.

O_NONBLOCK: In non-blocking mode, socket operations such as accept(), recv(), and send() return immediately, even if there is no data available.
 without blocking the execution flow.
*/

void AcceptorSockets::setSocketNonBlocking() {
    int flags = fcntl(_AcceptorSocketFd, F_GETFL, 0);
    if (flags == -1) {
        throw std::runtime_error("fcntl() failed");
    }

    flags |= O_NONBLOCK;
    if (fcntl(_AcceptorSocketFd, F_SETFL, flags) == -1) {
        throw std::runtime_error("fcntl() failed");
    }
}

void AcceptorSockets::bind_socket()
{
    if (bind(_AcceptorSocketFd, (struct sockaddr *)&_addr, sizeof(_addr)) == -1)
    {
        throw std::runtime_error("bind() failed");
    }
}

void AcceptorSockets::listen_socket()
{
    if (listen(_AcceptorSocketFd, this->backlogQueueMax) == -1)
    {
        throw std::runtime_error("listen() failed");
    }
}

/*
    (EINTR), non-blocking operation (EAGAIN), or no connections available (EWOULDBLOCK).
*/
int AcceptorSockets::accept_socket()
{
    int newClientFd = accept(_AcceptorSocketFd, (struct sockaddr *)&_addr, &_addrlen);
    std::cout << "newClient just connected: " << newClientFd << std::endl;
    if (newClientFd == -1)
    {
        if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
        {
            throw std::runtime_error("accept() Just for check");
        }
        else
        {
            throw std::runtime_error("accept() failed");
        }
        return -1;
    }
    if (!checkMaxClients())
    {
        close(newClientFd);
        return -503; // HTTP status code 503 indicates that the server is temporarily unable to handle the request due to being overloaded 
    }
    this->_clientsFD.push_back(newClientFd);
    return newClientFd;
}

bool AcceptorSockets::checkMaxClients()
{
    // std::cout << "this->_clientsFD.size() = " << this->_clientsFD.size() << " backLogQueueMax = " << this->backlogQueueMax << std::endl;
    printClientsFDs();
    if (this->_clientsFD.size() >= this->backlogQueueMax)
    {
        std::cerr << "[ Max clients connections reached ] \n";
        return false;
    }
    return true;
}

void AcceptorSockets::removeClient(int client)
{
    std::vector<int>::iterator it = this->_clientsFD.begin();
    while (it != this->_clientsFD.end())
    {
        if (*it == client)
        {
            this->_clientsFD.erase(it);
            break;
        }
        ++it;
    }
}