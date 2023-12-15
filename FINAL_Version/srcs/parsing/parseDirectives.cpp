
#include <Parsing.hpp>
 
std::vector<std::string>		getAllowedMethods(std::string& value, const std::string& key) {
	std::string					token;
	std::vector<std::string>	allowedMethods;
		
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	std::istringstream			toSplit(value);
	while (std::getline(toSplit, token, ' ')) {
		if (token == "") continue;
		if (token != "GET" && token != "POST" && token != "DELETE") {
			std::cerr << INVALID_METHOD;
			std::cerr << PRINT_LINE_AND_FILE;
			exit(1);
		}
		allowedMethods.push_back(token);
	}
	return (allowedMethods);
}

std::string	getIndex(std::string& value, const std::string& key) {
	// value = trim(value);
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	return (value);
}

std::string	getUploadPath(std::string& value, const std::string& key) {
	// value = trim(value);
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	return (value);
}

bool	getAutoIndex(std::string& value, const std::string& key) {
	// value = trim(value);
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	if (value == "0" || value == "false" || value == "FALSE" || value == "off" || value == "OFF") {
		return (false);
	} else if (value == "1" || value == "true" || value == "TRUE" || value == "on" || value == "ON") {
		return (true);
	} else {
		std::cerr << INVALID_ARGUMENT;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
}

void	getRedirect(std::string& value, const std::string& key, int& redirectionCode, std::string& redirectTo) {
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}

	size_t first = value.find_first_of(",");
	if (first == std::string::npos) {
		std::cerr << INVALID_ARGUMENT;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	std::string	returnIndex = trim(value.substr(0, first));
	std::string returnValue = trim(value.substr(first + 1, -1));
	if (returnIndex == "" || returnValue == ""
		|| returnIndex.find_first_not_of("0123456789") != value.npos) {
		std::cerr << INVALID_ARGUMENT;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	redirectionCode = atoi(returnIndex.c_str());
	redirectTo = trim(value.substr(first + 1, -1));
	// size_t first = value.find_first_of(",");
	// if (first != std::string::npos) {
	// 	redirectionCode = atoi(trim(value.substr(0, first)).c_str());
	// 	redirectTo = trim(value.substr(first + 1, -1));
	// }
	// if (redirectFrom == "" || redirectTo == "") {
	// 	std::cerr << INVALID_ARGUMENT;
	// 	std::cerr << PRINT_LINE_AND_FILE;
	// 	exit(1);
	// }
}

std::string	getRoot(std::string& value, const std::string& key) {
	value = trim(value);
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	return (value);
}

std::string getServerName(std::string& value, const std::string& key) {
	value = trim(value);
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	return (value);
}

static std::string	parseIpAddress(std::string& ipAdd, const std::string& key) {
	if (ipAdd == "localhost") return ("127.0.0.1");
	if (ipAdd.find_first_not_of("0123456789.") != ipAdd.npos
		|| std::count(ipAdd.begin(), ipAdd.end(), '.') != 3) {
		std::cerr << INVALID_ARGUMENT;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	std::istringstream	iss(ipAdd);
	std::string			tmp;
	while (std::getline(iss, tmp, '.')) {
		if (tmp.length() < 4 && tmp.find_first_not_of("0123456789") != tmp.npos) {
			if (std::atoi(tmp.c_str()) > 255) {
				std::cerr << INVALID_ARGUMENT;
				std::cerr << PRINT_LINE_AND_FILE;
				exit(1);
			}
		}
	}
	return (ipAdd);
}

static int	parsePort(std::string& port, const std::string& key) {
	if (port.find_first_not_of("0123456789") != port.npos) {
		std::cerr << INVALID_ARGUMENT;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	int portNumber = std::atoi(port.c_str());
	if (portNumber < 1 || portNumber > 65535) {
		std::cerr << INVALID_ARGUMENT;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	return (portNumber);
}

int	getPortAndIpAddress(std::string& value, const std::string& key, std::string& ipAddress) {
	value = trim(value);
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}

	size_t	sep = value.find(':');
	if (sep != value.npos) {
		std::string	ipAdd = value.substr(0, sep);
		std::string port = value.substr(sep + 1, -1);
		if (ipAdd.empty() || port.empty()) {
			std::cerr << INVALID_ARGUMENT;
			std::cerr << PRINT_LINE_AND_FILE;
			exit(1);
		}
		ipAddress = parseIpAddress(ipAdd, key);
		return (parsePort(port, key));
	} else {
		if (value == "localhost") {
			ipAddress = "127.0.0.1";
			return (8080);
		}
		if (value.find('.') != value.npos) {
			ipAddress = parseIpAddress(value, key);
			return (8080);
		} else if (value.find_first_not_of("0123456789") == value.npos) {
			ipAddress = "0.0.0.0";
			return (parsePort(value, key));
		} else {
			std::cerr << INVALID_ARGUMENT;
			std::cerr << PRINT_LINE_AND_FILE;
			exit(1);
		}
	}
}

void	getErrorPages(std::string& value, const std::string& key, std::map<int, std::string>& errorPages) {
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	std::string			token;
	std::istringstream	em(value);
	while (std::getline(em, token)) {
		token = trim(token);
		size_t first = token.find_first_of(",");
		if (first == std::string::npos) {
			std::cerr << INVALID_ARGUMENT;
			std::cerr << PRINT_LINE_AND_FILE;
			exit(1);
		}
		std::string	errorIndex = trim(token.substr(0, first));
		std::string errorValue = trim(token.substr(first + 1, -1));
		if (errorIndex == "" || errorValue == ""
			|| errorIndex.find_first_not_of("0123456789") != value.npos) {
			std::cerr << INVALID_ARGUMENT;
			std::cerr << PRINT_LINE_AND_FILE;
			exit(1);
		}
		// if (errorPages[std::atoi(errorIndex.c_str())].empty()) {
		errorPages[std::atoi(errorIndex.c_str())] = errorValue;
		// }
	}
}

unsigned long long	getLimitClientBody(std::string& value, const std::string& key) {
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	value = trim(value);
	char	suffix = value[value.size() - 1];
	unsigned long long	numericValue = std::atoll(value.c_str());
    switch (suffix) {
        case 'M':
        case 'm':
            return numericValue * 1024 * 1024; // Megabytes to bytes
        case 'K':
        case 'k':
            return numericValue * 1024; // Kilobytes to bytes
        case 'G':
        case 'g':
            return numericValue * 1024 * 1024 * 1024; // Gigabytes to bytes
        default:
            return numericValue; // No suffix, treat as bytes
    }
	return (0);
}

std::string	getCgiExecutable(std::string& value, const std::string& key) {
	if (value.empty() || value == ";") {
		std::cerr << NO_VALUE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	return (value);
}

