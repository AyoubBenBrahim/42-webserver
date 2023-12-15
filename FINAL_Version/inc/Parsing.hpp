#pragma once 

# include <sys/socket.h>
# include <stdlib.h>
# include <unistd.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <signal.h>
# include <iostream>
# include <fstream>
# include <string>
# include <vector>
# include <map>
# include <sstream>
# include <macros.hpp>
# include <stack>

// Request			*requestParse(std::string buffer);

typedef struct LocationDirectives {
	int							redirectionCode;
	bool						autoindex;
	bool                        isCgi;
	unsigned long long			clientMaxBodySize;
	std::string                 uploadPath;
	std::string                 cgiExecutable;
	std::string					path;
	std::string					root;
	std::string					index;
	std::string 				redirectTo;
	std::vector<std::string>	allowedMethods;
	std::map<int, std::string> 	errorPages;
}								t_location;

typedef struct ServerDirectives {
	int								port;
	int								redirectionCode;
	bool							autoindex;
	unsigned long long				clientMaxBodySize;
	std::string                 	uploadPath;
	std::string						ipAddress;
	std::string						serverName;
	std::string						root;
	std::string						index;
	std::string 					redirectTo;
	std::string                 	cgiExecutable;
	std::vector<std::string>        allowedMethods;
	std::vector<t_location>     	locations;
	std::map<int, std::string> 		errorPages;
}									t_server;

typedef struct ConfigSettings {
	std::vector<t_server>	servers;
}							t_config;

/* extra functions */
std::string					trim(const std::string& str);
bool						bracketsBalance(const std::string& str);

/* Parsing Directives */
void                    	getErrorPages(std::string& value, const std::string& key, std::map<int, std::string>& errorPages);
std::vector<std::string>	getAllowedMethods(std::string& value, const std::string& key);
std::string					getIndex(std::string& value, const std::string& key);
std::string					getRoot(std::string& value, const std::string& key);
std::string					getUploadPath(std::string& value, const std::string& key);
std::string					getCgiExecutable(std::string& value, const std::string& key);
std::string					getServerName(std::string& value, const std::string& key);
void						getRedirect(std::string& value, const std::string& key, int& redirectionCode, std::string& redirectTo);
bool						getAutoIndex(std::string& value, const std::string& key);
int							getPortAndIpAddress(std::string& value, const std::string& key, std::string& ipAddress);
unsigned long long			getLimitClientBody(std::string& value, const std::string& key);

/* Parse Locations */
void						parseLocationDirectives(std::string& key, std::string& value, t_location& location);


/* Parse Servers */
void						parseServerDirectives(std::string& key, std::string& value, t_server& server);
void						parseServerLines(std::string& line, t_server& server);
t_server					parseServerBlock(std::string line);

/* Parse Config File */
t_config					parseConFile(const char* file);
void						parseLocationLines(std::string& line, t_location& location);
t_location					parseLocationBlock(std::string line);

/* Parse Request */
// std::map<std::string, std::string>  fillContentTypeMap();
// void	correctPath(std::string& path); // just for now
std::vector<std::string>	splitLine(std::string line, std::string delimiter);


void		parseHead(std::map<std::string, std::string>& headMap, std::vector<std::string> lines);
void		parseStartLine(std::map<std::string, std::string> &headMap, std::string line);
void		checkRequest(std::string	buffer);
void		checkPath(std::string path, std::string connection);
void		unchunkBody(std::string& body, std::string boundary);