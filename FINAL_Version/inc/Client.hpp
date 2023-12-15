#pragma once

#include <Response.hpp>
#include <sys/event.h>

# define READ 0
# define WRITE 1

class Client {
    private :
        int             status;
        Request         *request;
        Response        *response;
        t_config        config;
        int kq;
        int fd;

    public :
        Client(int kq, int fd, t_config config);
        void                    setRequest(Request* request);
        void                    setResponse(Response* response);
        Request*                getRequest();
        int                     getClientFd();
        Response*               getResponse();
        void                    setStatus(int status);
        int                     getKq() const;
        Request*	createNewRequest();
};