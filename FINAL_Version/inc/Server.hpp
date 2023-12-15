#pragma once

#include "AcceptorSockets.hpp"
#include <Client.hpp>

class Server
{
private:
	t_config						config;
	std::map<int, AcceptorSockets>	acceptorSockets;
	std::map<int, Client>			clients;
private :
	void	closeAllFds();
public:
	Server();
	Server(t_config& config);
	~Server();

	void    setupServerConnections();
	void    runServer();
	void    read_socket(Client& client, int eventData);
	bool 	write_socket(Client& client);
	void    kqueueEventHandler();
	void    clientDisconnected(int clientFD);
};
