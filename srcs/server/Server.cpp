#include "Server.hpp"

// Server::Server(const Server &src)
// {
//     *this = src;
// }

// Server::Server()
// {
//     _addrlen = sizeof(_addr);
// }

// Server &Server::operator=(const Server &src)
// {
//     if (this != &src)
//     {
//         this->_socket = src.getSocket();
//         this->_addr = src.getSockAddr();
//         this->_addrlen = src.getAddrLen();
//         std::vector<int> copy = src.getClient();
//         for (auto it = copy.begin(); it != copy.end(); it++)
//         {
//             this->_clients.push_back(*it);
//         }
//         this->_logFile = src.getLogFile();
//         for (int index = 0; index < MAX_CLIENTS; index++)
//         {
//             this->fds[index].fd = src.getFds()[index].fd;
//             this->fds[index].events = src.getFds()[index].events;
//             this->fds[index].revents = src.getFds()[index].revents;
//         }
//         this->_daemonPid = src.getDaemonPid();
//     }
//     return *this;
// }

int Server::getSocket() const
{
    return this->_socket;
}

struct sockaddr_in Server::getSockAddr() const
{
    return this->_addr;
}

socklen_t Server::getAddrLen() const
{
    return this->_addrlen;
}

std::vector<int> Server::getClient() const
{
    return this->_clients;
}

const struct pollfd *Server::getFds() const
{
    return this->fds;
}

void Server::create_socket()
{
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == -1)
    {
        throw std::runtime_error("socket() failed");
    }
}

void Server::bind_socket()
{
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(LISTEN_PORT);
    _addr.sin_addr.s_addr = INADDR_ANY;

    // Set SO_REUSEADDR option
    int reuseAddr = 1;
    if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr)) == -1)
    {
        throw std::runtime_error("setsockopt() failed");
    }

    if (bind(_socket, (struct sockaddr *)&_addr, sizeof(_addr)) == -1)
    {
        throw std::runtime_error("bind() failed");
    }

    if (fcntl(_socket, F_SETFL, O_NONBLOCK) == -1)
    {
        throw std::runtime_error("fcntl() failed");
    }
}

void Server::listen_socket()
{
    if (listen(_socket, MAX_CLIENTS) == -1)
    {
        throw std::runtime_error("listen() failed");
    }
}

bool Server::accept_socket()
{
    int newClient = accept(_socket, (struct sockaddr *)&_addr, &_addrlen);
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
    _clients.push_back(newClient);
    return true;
}

void Server::read_socket(int client, int *clientCount)
{
    char buffer[1024] = {0};
    int valread = read(client, buffer, sizeof(buffer));
    if (valread == -1)
    {
        throw std::runtime_error("read() failed");
    }
    if (valread == 0)
    {
        for (std::vector<int>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
        {
            if (*it == client)
            {
                this->_clients.erase(it);
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

void Server::write_socket(int client, const std::string& message) {
    int valwrite = write(client, message.c_str(), message.length());
    if (valwrite == -1) {
        throw std::runtime_error("write() failed");
    }
    std::cout << "Sent response to client " << client << ": " << message << std::endl;
    std::string msg = "hi hello";
    send(client, msg.c_str(), msg.length(), 0);
}

// void Server::write_socket(int client)
// {
//     std::string hello = "Hello from server";
//     send(client, hello.c_str(), hello.length(), 0);
// }

void Server::close_socket()
{
    close(_socket);
}

bool Server::checkMaxClients()
{
    if (this->_clients.size() >= MAX_CLIENTS)
    {
        std::cout << "ERROR: Max clients reached\n";
        return false;
    }
    return true;
}

void Server::run()
{
    create_socket();
    bind_socket();
    listen_socket();

    memset(fds, 0, sizeof(fds));

    fds[0].fd = _socket;
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
            fds[++clientCount].fd = _clients.back();
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

size_t Server::getClientsCount() const
{
    return _clients.size();
}

void Server::shiftArray(int *size)
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

Server::~Server()
{
}
