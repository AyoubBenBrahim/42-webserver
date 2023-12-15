#include <Client.hpp>

Client::Client(int kq, int fd, t_config config) {
	this->kq = kq;
	this->fd = fd;
	this->config = config;
	this->response = NULL;
	status = GOOD_STATUS;
	request = NULL;
}

void Client::setRequest(Request* request) {
	this->request = request;
}


int	Client::getKq() const {
	return (this->kq);
}

int   Client::getClientFd() {
	return (this->fd);
}


Request*	Client::createNewRequest() {
	return new Request();
}

Request    *Client::getRequest() {
	return (this->request);
}

Response*	Client::getResponse() {
	if (!response) {
		if (!status) {
			response = new Response(this->request, config);
		}
		else
			response = new Response(status, config);
	}
	return (this->response);
}
void                    Client::setResponse(Response* response) {
	this->response = response;
}

void	Client::setStatus(int status) {
	this->status = status;
}
