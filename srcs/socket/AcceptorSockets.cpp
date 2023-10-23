#include "AcceptorSockets.hpp"


AcceptorSockets::AcceptorSockets(in_addr host, int port)
{
    this->listen_port = port;
    // this->backlogQueueMax = 3;
    this->_addrlen = sizeof(_addr);
    this->host = host;
}

AcceptorSockets::~AcceptorSockets()
{
}

AcceptorSockets::~AcceptorSockets()
{
}   

AcceptorSockets &AcceptorSockets::operator=(const AcceptorSockets &src)
{
    if (this != &src)
    {
        this->_socketFd = src.getSocketFd();
        this->_addr = src.getSockAddr();
        this->_addrlen = src.getAddrLen();
        std::vector<int> copy = src.getClient();
        for (auto it = copy.begin(); it != copy.end(); it++)
        {
            this->_clientsFD.push_back(*it);
        }

        // for (int index = 0; index < MAX_CLIENTS; index++)
        // {
        //     this->fds[index].fd = src.getFds()[index].fd;
        //     this->fds[index].events = src.getFds()[index].events;
        //     this->fds[index].revents = src.getFds()[index].revents;
        // }
    }
    return *this;
}

int AcceptorSockets::getSocketFd() const
{
    return this->_socketFd;
}

struct sockaddr_in AcceptorSockets::getSockAddr() const
{
    return this->_addr;
}

socklen_t AcceptorSockets::getAddrLen() const
{
    return this->_addrlen;
}

std::vector<int> AcceptorSockets::getClient() const
{
    return this->_clientsFD;
}

const struct pollfd *AcceptorSockets::getFds() const
{
    return this->fds;
}

void AcceptorSockets::create_socket()
{
    _socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_socketFd == -1)
    {
        throw std::runtime_error("socket() failed");
    }
}

void AcceptorSockets::bind_socket()
{
    /*
        _addr.sin_addr.s_addr = inet_addr(host.c_str());
        assigns the IP address specified by the host variable

        INADDR_ANY represents the IP address "0.0.0.0",
        indicating that the server socket should bind and
        listen on all available network interfaces on the system.
        This allows the server to accept incoming connections
        on any IP address associated with the machine.
    */

    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(this->listen_port);
    _addr.sin_addr.s_addr = INADDR_ANY;
    // _addr.sin_addr.s_addr = host.s_addr;
    // memcpy(&_addr.sin_addr, &host, sizeof(host));

    // Set SO_REUSEADDR option
    int reuseAddr = 1;
    if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr)) == -1)
    {
        throw std::runtime_error("setsockopt() failed");
    }

    if (bind(_socketFd, (struct sockaddr *)&_addr, sizeof(_addr)) == -1)
    {
        throw std::runtime_error("bind() failed");
    }

    if (fcntl(_socketFd, F_SETFL, O_NONBLOCK) == -1)
    {
        throw std::runtime_error("fcntl() failed");
    }
}

void AcceptorSockets::listen_socket()
{
    if (listen(_socketFd, this->backlogQueueMax) == -1)
    {
        throw std::runtime_error("listen() failed");
    }
}

bool AcceptorSockets::accept_socket()
{
    int newClient = accept(_socketFd, (struct sockaddr *)&_addr, &_addrlen);
    if (newClient == -1)
    {
        if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
        {
            throw std::runtime_error("accept() Just for check");
        }
        else
        {
            throw std::runtime_error("accept() failed");
        }
    }
    if (!checkMaxClients())
    {
        close(newClient);
        return false;
    }
    _clientsFD.push_back(newClient);
    return true;
}

void AcceptorSockets::read_socket(int client, int *clientCount)
{
    char buffer[1024] = {0};
    int valread = read(client, buffer, sizeof(buffer));
    if (valread == -1)
    {
        throw std::runtime_error("read() failed");
    }
    if (valread == 0)
    {
        for (std::vector<int>::iterator it = this->_clientsFD.begin(); it != this->_clientsFD.end(); it++)
        {
            if (*it == client)
            {
                this->_clientsFD.erase(it);
                break;
            }
        }
        this->fds[*clientCount].fd = -1;
        this->shiftArray(clientCount);
        close(client);
    }
    else
    {
        // std::string message = buffer;
        // std::cout << message << std::endl;
        // if (message == "quit\n")
        // {
        //     for (auto it = this->_clients.begin(); it != this->_clients.end(); it++)
        //         close(*it);
        //     close(this->fds[0].fd);
        //     // Tintin_reporter::getInstance().log_message_1("INFO", "Server", "Quitting");
        //     // std::remove("/var/lock/Server.lock");
        //     exit(1);
        // }
        // Tintin_reporter::getInstance().log_message_1("LOG", "Server", "User input: " + message);
        // Process the received message
        std::string message(buffer);
        std::cout << "Received message from client " << client << ": " << message << std::endl;

        // Send a response back to the client
        std::string response = "Server received your message: " + message;
        write_socket(client, response);
    }
}

void AcceptorSockets::write_socket(int client, const std::string& message) {
    int valwrite = write(client, message.c_str(), message.length());
    if (valwrite == -1) {
        throw std::runtime_error("write() failed");
    }
    std::cout << "Sent response to client " << client << ": " << message << std::endl;
    std::string msg = "hi hello";
    send(client, msg.c_str(), msg.length(), 0);
}

// void AcceptorSockets::write_socket(int client)
// {
//     std::string hello = "Hello from server";
//     send(client, hello.c_str(), hello.length(), 0);
// }

void AcceptorSockets::close_socket()
{
    close(_socketFd);
}

bool AcceptorSockets::checkMaxClients()
{
    if (this->_clientsFD.size() >= this->backlogQueueMax)
    {
        std::cout << "ERROR: Max clients connections reached\n";
        return false;
    }
    return true;
}

void AcceptorSockets::run()
{
    // create_socket();
    // bind_socket();
    // listen_socket();

    memset(fds, 0, sizeof(fds));

    fds[0].fd = _socketFd;
    fds[0].events = POLLIN;

    int clientCount = 0;

    while (true)
    {
        int result = poll(fds, clientCount + 1, -1);
        if (result == -1)
        {
            if (errno == EINTR)
                continue;
            throw std::runtime_error("poll() failed");
        }
        if (fds[0].revents & POLLIN)
        {
            if (!accept_socket())
                continue;
            fds[++clientCount].fd = _clientsFD.back();
            fds[clientCount].events = POLLIN;
        }

        for (int i = 1; i <= clientCount; i++)
        {
            if (fds[i].revents & POLLIN)
                read_socket(fds[i].fd, &clientCount);
        }
    }

    close_socket();
}

size_t AcceptorSockets::getClientsCount() const
{
    return _clientsFD.size();
}

void AcceptorSockets::shiftArray(int *size)
{
    int i = 0;
    int n = *size;

    while (i < n)
    {
        if (fds[i].fd == -1)
        {
            for (int j = i; j < n - 1; j++)
            {
                fds[j].fd = fds[j + 1].fd;
                fds[j].events = fds[j + 1].events;
            }
            n--;
        }
        else
            i++;
    }

    *size = n;
}