
#include <Parsing.hpp>

void	parseServerDirectives(std::string& key, std::string& value, t_server& server) {
// static bool listenDup = false;
	// static bool rootDup = false;

	value.erase(value.length() - 1, 1);
	if (key == "server_name") {
		server.serverName = getServerName(value, key);
	} else if (key == "listen") {
// if (listenDup) {
		// 	std::cerr << RED "Error: " GREEN "A duplicate listen a server.\n" RESET_COLOR;
		// 	std::cerr << PRINT_LINE_AND_FILE;
		// 	std::exit(1);
		// }
		server.port = getPortAndIpAddress(value, key, server.ipAddress);
// listenDup = true;
	} else if (key == "root") {
// if (rootDup) {
		// 	std::cerr << RED "Error: " GREEN "A duplicate root a server.\n" RESET_COLOR;
		// 	std::cerr << PRINT_LINE_AND_FILE;
		// 	std::exit(1);
		// }
		server.root = getRoot(value, key);
// rootDup = true;
	} else if (key == "index") {
		server.index = getIndex(value, key);
	} else if (key == "error_page") {
		getErrorPages(value, key, server.errorPages);
	} else if (key == "return") {
		getRedirect(value, key, server.redirectionCode, server.redirectTo);
	} else if (key == "limit_client_body") {
		server.clientMaxBodySize = getLimitClientBody(value, key);
	} else if (key == "allowed_methods") {
		server.allowedMethods = getAllowedMethods(value, key);
	} else if (key == "autoindex") {
		server.autoindex = getAutoIndex(value, key);
	} else if (key == "upload_path") {
		server.uploadPath = getUploadPath(value, key);
	} else if (key == "cgi_executable") {
		server.cgiExecutable = getCgiExecutable(value, key);
	} else {
		std::cerr << INVALID_DIRECTIVE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
}

void	parseServerLines(std::string& line, t_server& server) {
	size_t sem = line.find(';');
	if (sem == line.npos && line.size() > 0) {
		std::cerr << EXPECTED_SEM;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	std::string semLine = line.substr(0, sem + 1);
	size_t eq = semLine.find('=');
	if (eq == semLine.npos) {
		std::cerr << INVALID_LOC_DIRECTIVE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	std::string	key = trim(semLine.substr(0, eq));
	std::string	value = trim(semLine.substr(eq + 1, -1));
	if (value.find('=') != value.npos) {
		std::cerr << EXPECTED_SEM;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	parseServerDirectives(key, value, server);
}

static void	serverDefaultValues(t_server& server) {
	server.autoindex = false;
	server.clientMaxBodySize = 0;
	// server.ipAddress = "0.0.0.0";
	server.port = -1;
	server.redirectionCode = -1;
	// server.redirectTo = "";
	// server.root = "";
	// server.index = "";
	// server.serverName = "";
}

t_server	parseServerBlock(std::string line) {
	t_server				server;
	std::vector<t_location>	locations;

	line = trim(line);
	serverDefaultValues(server);
	while (line.length() > 0) {
		size_t sem = line.find(';');
		if (sem == line.npos && line.size() > 0) {
			std::cerr << EXPECTED_SEM;
			std::cerr << PRINT_LINE_AND_FILE;
			exit(1);
		}
		std::string semLine = line.substr(0, sem + 1);
		size_t FirstBracket = semLine.find('{');
		if (FirstBracket != semLine.npos) {
			size_t nextBracket = line.find('}');
			semLine = line.substr(0, nextBracket + 1);
			locations.push_back(parseLocationBlock(trim(semLine)));
			line.erase(line.find(semLine), semLine.length());
			continue ;
		} else {
			semLine = trim(semLine);
			parseServerLines(semLine, server);
			line.erase(0, sem + 1);
		}
	}
	server.locations = locations;
	if (server.port == -1 || server.ipAddress.empty()) {
		std::cerr << RED "Error: " GREEN "Listen directive is necessary!" RESET_COLOR << "\n";
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	return(server);
}
