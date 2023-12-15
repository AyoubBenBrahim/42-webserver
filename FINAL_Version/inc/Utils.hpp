#pragma once

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <netdb.h>
#include <arpa/inet.h>
#include <stack>

std::vector<std::string>	splitLine(std::string line, std::string delimiter);
std::string					trim(const std::string& str);
std::string                 to_string(long long num);
const std::string			getExtension(const std::string& path);
std::string					getHostInfo(const char* host, const char *port);
bool						bracketsBalance(const std::string& str);
bool 						isDirectory(const char* path);


// for testing
void						check_leaks();
std::string					intToHex(int number);
