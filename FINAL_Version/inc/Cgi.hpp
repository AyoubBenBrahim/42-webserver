#pragma once 
#include <string>
#include <map>
#include <signal.h>
#include <unistd.h>
#include <Request.hpp>


class Cgi {
    private :
        int pid;
        char **env;
        char *program[3];
		int		in;
		int		out;
        int     timeout;
    public :
        Cgi(Request* request, const std::string& cgiExecutable, const std::string& scriptPath);
        ~Cgi();
        char **getEnv();

        char **getProgram();
		void setPid(int pid);
		int getPid();
		void setIn(int in);
		int getIn();
		void setOut(int out);
		int getOut();
        int getTimeout();
};