#include <Parsing.hpp>
 
void	parseLocationDirectives(std::string& key, std::string& value, t_location& location) {
// static bool rootDup = false;
	value.erase(value.length() - 1, 1);
	if (key == "allowed_methods") {
		location.allowedMethods = getAllowedMethods(value, key);
	} else if (key == "index") {
		location.index = getIndex(value, key);
	} else if (key == "autoindex") {
		location.autoindex = getAutoIndex(value, key);
	} else if (key == "upload_path") {
		location.uploadPath = getUploadPath(value, key);
	} else if (key == "error_page") {
		getErrorPages(value, key, location.errorPages);
	} else if (key == "limit_client_body") {
		location.clientMaxBodySize = getLimitClientBody(value, key);
	} else if (key == "root") {
// if (rootDup) {
		// 	std::cerr << RED "Error: " GREEN "A duplicate root in a location.\n" RESET_COLOR;
		// 	std::cerr << PRINT_LINE_AND_FILE;
		// 	std::exit(1);
		// }
		location.root = getRoot(value, key);
// rootDup = true;
	} else if (key == "return") {
		getRedirect(value, key, location.redirectionCode, location.redirectTo);
	} else if (key == "cgi_executable") {
		location.cgiExecutable = getCgiExecutable(value, key);
	} else {
		std::cerr << INVALID_LOC_DIRECTIVE;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
}

void	parseLocationLines(std::string& line, t_location& location) {
	while (line.length() > 0) {
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
		parseLocationDirectives(key, value, location);
		line.erase(0, sem + 1);
	}
}

static void	locationDefaultValues(t_location& location) {
	location.autoindex = 0;
	location.isCgi = 0;
	location.redirectionCode = -1;
	location.clientMaxBodySize = 0;
	// location.cgiExecutable = "";
	// location.index = "";
	// location.path = "";
	// location.redirectTo = "";
}


t_location	parseLocationBlock(std::string line) {
	t_location location;

	locationDefaultValues(location);
	if (line.substr(0, 8) != "location") {
		std::cerr << GREEN << "Invalid location Block!\n" << RESET_COLOR;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	line.erase(0, 8);
	line.erase(line.length() - 1, 1);
	line = trim(line);
	size_t bracket = line.find('{');
	if (bracket == 0) {
		std::cerr << NO_LOC_PATH;
		std::cerr << PRINT_LINE_AND_FILE;
		exit(1);
	}
	location.path = trim(line.substr(0, bracket));
	
	line = trim(line.substr(bracket + 1, -1));
	parseLocationLines(line, location);
	return (location);
}