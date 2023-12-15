#include <Parsing.hpp>

static int findMatchingOpenCurlyBrace(const std::string& text) {
    int nestingLevel = 0;

    for (size_t i = 0; i < text.length(); i++) {
        if (text[i] == '{') {
            nestingLevel++;
        } else if (text[i] == '}') {
            nestingLevel--;
            if (nestingLevel == 0) {
                return (i);  // Found the matching open curly brace
            }
        }
    }
    return (-1);  // No matching open curly brace found
}

/*
	This function processes an input configuration file, removing comments and unnecessary whitespace,
	and checks the balance of curly brackets. It storesult the cleaned configuration data in the 'res' string.
	it prints corresponding error messages to the standard error stream and exits with an error code.
*/
static void	cleanFile(std::ifstream& configFile, std::string& result) {
	std::string line;
	while (std::getline(configFile, line)) {
		size_t commentPos = line.find("#");
		if (commentPos != std::string::npos) {
			line.erase(commentPos, line.length());
		}
		line = trim(line);
		if (line.empty()) {
			continue ;
		}
		result += line;
	}
	if (result.empty() || result.find('{') == result.npos) {
		std::cerr << RED "Error: " GREEN "invalid config file." RESET_COLOR << std::endl;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}

	if (!bracketsBalance(result)) {
		std::cerr << RED "Error: " GREEN "Unclosed bracket in configuration file." RESET_COLOR << std::endl;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
}

t_config	parseConFile(const char* file) {
	t_location	currentLocation;
	t_server	currentServer;
	t_config	config;
	std::string	line;
	std::string	result;

	std::ifstream configFile(file);
	if (configFile.fail()) {
		std::cerr << RED "Error: " GREEN "Failed to open the configuration file." RESET_COLOR << std::endl;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}

	cleanFile(configFile, result);
	while (result.size() > 0) {
		result = trim(result);
		if (trim(result.substr(0, 6)) != "server") {
			std::cerr << GREEN << "No Server Block Found!\n" << RESET_COLOR;
			std::cerr << PRINT_LINE_AND_FILE;
			exit(1);
		}
		result = trim(result.substr(6, -1));
		if (result[0] != '{') {
			std::cerr << PRINT_LINE_AND_FILE;
			exit(1);
		}
		int blockLen = findMatchingOpenCurlyBrace(result);
		if (blockLen == -1) {
			std::cerr << PRINT_LINE_AND_FILE;
			exit(1);
		}
		result.erase(0, 1); // remove the first bracket in the current server block
		result.erase(blockLen - 1, 1); // remove the last bracket in the current server block
		line = result.substr(0, blockLen - 1);
		config.servers.push_back(parseServerBlock(line));
		result.erase(result.find(line), line.length());
	}
	return (config);
}
