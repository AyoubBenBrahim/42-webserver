#include "Server.hpp"
#include <cassert>
#include <cstddef>
#include <iostream>
#include <sys/event.h>
#include <vector>
#include <macros.hpp>


Server::Server() {}

Server::Server(t_config& config) : config(config) {}

Server::~Server() {}

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

	try {
		this->kqueueEventHandler();
	} catch (const std::exception& ex) {
		
		std::cout << ex.what() << std::endl;
	}
}

void Server::setupServerConnections()
{
	for (size_t i = 0; i < config.servers.size(); ++i)
	{
		in_addr address;
		address.s_addr = inet_addr(config.servers[i].ipAddress.c_str());
		AcceptorSockets acceptor(address, config.servers[i].port);
		try {
			acceptor.socketAPI();
		} catch(const std::exception& e) {
			std::cerr << e.what() << '\n';
			continue ;
		}
		
		int socketFd = acceptor.getSocketFd();
		this->acceptorSockets.insert(std::pair<int, AcceptorSockets>(socketFd, acceptor));
	}
	if (!acceptorSockets.size()) {
		exit(1);
	}
}

void Server::closeAllFds()
{
	std::map<int, AcceptorSockets>::iterator	it = acceptorSockets.begin();

	for (; it != acceptorSockets.end(); it++)
		close(it->first);
}

static void	manageEventState(Client& client, int filter) {
	struct kevent clientEvent;
	int			deletedFilter;

	deletedFilter = (filter == EVFILT_READ) ? EVFILT_WRITE : EVFILT_READ;

	EV_SET(&clientEvent, client.getClientFd(), deletedFilter, EV_DELETE, 0, 0, NULL);
	if (kevent(client.getKq(), &clientEvent,1, 0, 0, NULL) == -1)
		throw std::runtime_error("kevent failed");

	EV_SET(&clientEvent, client.getClientFd(), filter, EV_ADD, 0, 0, NULL);
	if (kevent(client.getKq(), &clientEvent,1, 0, 0, NULL) == -1)
		throw std::runtime_error("kevent failed");
}

// udata : user-defined value passed through the kernel unchanged.
const uint64_t SERVER_UDATA = 1337;

void Server::kqueueEventHandler() {
	std::map<int, AcceptorSockets>::iterator it;
	std::map<int, AcceptorSockets>::iterator ite = this->acceptorSockets.end();

	int kq = kqueue();
	if (kq == -1) {
		std::cerr << "kqueue() failed\n";
		std::exit(1);
	}
	// Register acceptor sockets for reading events
	for (it = this->acceptorSockets.begin(); it != ite; ++it) {

		struct kevent serverEvent;
		EV_SET(&serverEvent, it->first, EVFILT_READ, EV_ADD, 0, 0, reinterpret_cast<void *>(SERVER_UDATA));
		int ret = kevent(kq, &serverEvent,1, 0, 0, NULL);
		if (ret == -1) {
			std::cerr << "kevent() failed to delete event\n";
			closeAllFds();
			std::exit(1);
		}
	}

	while (true) {
		std::vector<struct kevent> triggeredEvents(1024);
		int eventCount = kevent(kq, 0, 0, triggeredEvents.data(), 1024, NULL);
		if (eventCount == -1) {
			throw std::runtime_error("kevent() failed in event loop");
		}
		for (int i = 0; i < eventCount; ++i) {
			struct kevent& event = triggeredEvents[i];
			// if (event.flags & EV_EOF) {
			if (event.udata == reinterpret_cast<void*>(SERVER_UDATA)) {
				std::system("sleep 0.00005");
				int acceptorSocketFD = event.ident;
				it = this->acceptorSockets.find(acceptorSocketFD);
				if (it == this->acceptorSockets.end()) {
					std::cerr << "acceptor socket not found\n";
					continue;
				}
				int newClient = it->second.accept_socket();
				if (newClient == -1) {
					std::cerr << "accept() failed for server fd " << acceptorSocketFD << std::endl;
					continue;
				}
		
				// Add new client socket to the event list
				struct kevent clientEvent;
				Client client(kq, newClient, config);
				clients.insert(std::pair<int, Client>(newClient, client));

				EV_SET(&clientEvent, newClient, EVFILT_READ, EV_ADD, 0, 0, NULL);
				if (kevent(kq, &clientEvent, 1, 0, 0, 0) == -1)
					std::cerr << "kevent EVFILT_READ Error\n";
			} else {

				// // Event is for a client socket (data available to read)
				if (event.flags & EV_EOF) {
					// Client closed the connection
					clientDisconnected(event.ident);
					std::cout << "Client " << event.ident << " *** disconnected ****" << std::endl;
					continue;
				}

				int clientFd = event.ident;
				std::map<int, Client>::iterator it =  clients.find(clientFd);
				if (it == clients.end()) {
					std::cout << "Client not found\n";
				}
				else {
					if (event.filter == EVFILT_READ) {
						if (event.data) {
							try	{
								read_socket(it->second, event.data);
							} catch (const std::exception& ex) {
								delete it->second.getRequest();
								clientDisconnected(event.ident);
							}
						}
					}
					else if (event.filter == EVFILT_WRITE) {
						try	{
							if (!write_socket(it->second)) {
								clientDisconnected(event.ident);
								std::cout << "Client " << event.ident << " *** disconnected ****" << std::endl;
							}
						} catch (const std::exception& ex) {
							clientDisconnected(event.ident);
						}
					}
				}
		}

	}
			// Event is for the server socket (new client connection)
		}
	// }
}

void Server::clientDisconnected(int clientFD) {
	clients.erase(clientFD);
	close(clientFD);
}

std::string decode_chunked(const std::string& chunked_string) {
    std::string decoded_string;
    std::istringstream iss(chunked_string);

    while (true) {
        // Read the chunk size as a string
        std::string chunk_size_str;
        std::getline(iss, chunk_size_str, '\r');
        iss.ignore(); // Skip the '\n'

        // Convert the chunk size string to an integer
        std::istringstream size_stream(chunk_size_str);
        size_stream >> std::hex >> std::ws; // Handle any leading whitespace
        std::size_t chunk_size;
        size_stream >> chunk_size;

        // Check for the end of the chunked data
        if (chunk_size == 0)
            break;

        // Read the chunk data
        std::string chunk_data(chunk_size, '\0');
        iss.read(&chunk_data[0], chunk_size);

        // Skip the '\r\n' separator
        iss.ignore(2);

        // Append the chunk data to the decoded string
        decoded_string += chunk_data;
    }

    return decoded_string;
}




static void	initRequest(Client& client, int eventData) {
	// (void)eventData;
	char	buffer[BUFFER_SIZE + 1];

	bzero(buffer, BUFFER_SIZE + 1);
	int len = read(client.getClientFd(), buffer, BUFFER_SIZE);
	if (len == -1) {
		throw std::runtime_error("read() failed");
	}
	if (!client.getRequest()->getHeadEnd()) {
		try {
			client.getRequest()->appendHead(buffer, len);
		} catch (int status) {
			client.setStatus(status);
		}
	}
	else {
		client.getRequest()->appendFile(buffer, len);
	}
	// if (client.getRequest()->getContentLength() == client.getRequest()->getTotal()) {
	if (eventData == len) {
		if (client.getRequest()->getValueByKey(REQ_TRANSFER) == "chunked") {
			std::string tmp = decode_chunked(client.getRequest()->getBody());
			client.getRequest()->setBody(tmp);
		}
		manageEventState(client, EVFILT_WRITE);
		if (!client.getRequest()->getHeadEnd()) {
			client.setStatus(BAD_REQUEST_STATUS);
		}
	}
}

void Server::read_socket(Client& client, int eventData) {
	if (!client.getRequest()) {
		client.setRequest(client.createNewRequest());
	}
	initRequest(client, eventData);
}

bool Server::write_socket(Client& client) {
	Request		*request = client.getRequest();
	Response	*response = client.getResponse();
	bool		keep_alive = request->getValueByKey(REQ_CONNECTION).empty() ? true : (request->getValueByKey(REQ_CONNECTION) == "keep-alive");
	
	std::string	_response;

	_response = response->getRequest() ? response->buildResponse() : response->getResponse();

	if (response->getUploaded() == true) {
		int len = write(client.getClientFd(), _response.c_str(), _response.length());
		if (len < BUFFER_SIZE) {
			delete response;
			client.setResponse(NULL);
			request->clear();
			delete request;
			client.setRequest(NULL);
			if (len == -1)
				return (false);
			manageEventState(client, EVFILT_READ);
			return (keep_alive);
		}
	}
	return (true);
}
