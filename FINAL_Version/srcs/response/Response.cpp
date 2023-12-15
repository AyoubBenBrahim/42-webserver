#include <Response.hpp>

void	Response::checkCgiHeader() {
	size_t http_version = header.find("HTTP/1.1");
	if (http_version == std::string::npos)
		header.insert(0, "HTTP/1.1 200 OK\r\n");
	size_t content_type = header.find("Content-Type:");
	if (content_type == std::string::npos) {
		size_t location_header = header.find("Location:");
		if (location_header == std::string::npos) {
			std::cout << "HEADER is not correcto\n";
			throw std::runtime_error(returnError(INTERNAL_SERVER_ERROR_STATUS));
		}
	}

	if (header.find("Content-Length") == std::string::npos) {
		header.insert(header.length() - 4, "\r\nTransfer-Encoding: chunked");
		chunked = true;

		body.insert(0, intToHex(body.length()) + "\r\n");
		body.insert(body.length(), "\r\n");
	}

}

bool	Response::isItChunked(){
	return (chunked);
}

void	Response::setResToChunked(bool chunked){
	this->chunked = chunked;
}

void	Response::executeCgi(const std::string& path, const std::string& cgi_executable) {
	if (!cgi) {
		int fd[2];
        pipe(fd);
        int pid = fork();
        cgi = new Cgi(request, cgi_executable, path);
        cgi->setIn(fd[0]);
        cgi->setOut(fd[1]);
        cgi->setPid(pid);
		if (pid) {
			if (write(fd[1], request->getBody().c_str(), request->getBody().length()) == -1)
				throw std::runtime_error(returnError(INTERNAL_SERVER_ERROR_STATUS));
		}
		if (!pid) {
			dup2(fd[0], STDIN_FILENO);
            dup2(fd[1], STDOUT_FILENO);
            close(fd[0]);
            close(fd[1]);
            execve(cgi->getProgram()[0], cgi->getProgram(), cgi->getEnv());
            perror("execve : ");
            exit(1);
		}
		int status;
		clock_t start_time = clock() / 1000000;
		int res = 0;
		do {
			clock_t current_time = clock() / 1000000;
			if (current_time - start_time > static_cast<size_t>(cgi->getTimeout()))
				break ;
			res = waitpid(cgi->getPid(), &status, WNOHANG);
		} while (!res);

		if (!res) {
			throw std::runtime_error(returnError(GATEWAY_TIMEOUT_STATUS));
		}
		if (WIFEXITED(status)) {
			status = WEXITSTATUS(status);
		}
		if (status) {
			std::cout << "EXIT WITH !0 status\n";
			if (access(path.c_str(), F_OK))
				throw (std::runtime_error(returnError(NOT_FOUND_STATUS)));
			throw std::runtime_error(returnError(INTERNAL_SERVER_ERROR_STATUS));
		}
	}

	char buffer[BUFFER_SIZE + 1];
	bzero(buffer, BUFFER_SIZE + 1);
	fcntl(cgi->getIn(), F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	int b = read(cgi->getIn(), buffer, BUFFER_SIZE);
	if (b == -1)
		throw std::runtime_error(returnError(INTERNAL_SERVER_ERROR_STATUS));
	body.clear();
	body.append(buffer, b);
	if (!headerDone) {
		size_t findEndOfHead = body.find("\r\n\r\n");
		if (findEndOfHead != body.npos) {
			header.insert(0, body.substr(0, findEndOfHead + 4));
			body = body.substr(findEndOfHead + 4, -1);
		}
		checkCgiHeader();
		headerDone = true;
		response = header + body;
	}
	else {
		if (chunked) {		
			body.insert(0, intToHex(body.length()) + "\r\n");
			body.insert(body.length(), "\r\n");
		}
		response = body;
	}
	if (b < BUFFER_SIZE && chunked) {
		response.insert(response.length(), "0\r\n\r\n");
	}
}

std::string getHeadOfBoundary(std::string& readed) {
    size_t index;
    std::string head;
    size_t      len;

    len = readed.length();
    index = readed.find("\r\n\r\n");
    if (index == std::string::npos)
        return ("");
    try {
        head = readed.substr(0, index + 4);
    } catch (std::exception& ex) {
        std::cout << "HEAD\n";
    }
    readed = readed.substr(index + 4, len);
    return (head);
}

std::string getFileName(const std::string& head) {
    size_t filenamePos = head.find("filename=");
    if (filenamePos != std::string::npos) {
        // Extract the substring starting from "filename=" to the end
        std::string filenameSubstring = head.substr(filenamePos + 10);
 
        // Find the position of the closing double quote after the filename
        size_t quotePos = filenameSubstring.find(head[filenamePos + 9]);
        if (quotePos != std::string::npos) {
            // Extract the filename
            std::string filename = filenameSubstring.substr(0, quotePos);
			return (filename);
        } else
			throw (BAD_REQUEST_STATUS);
    } else {
		return ("");
    }
}

# define WRITE_BUFFER_SIZE 1000000

bool	Response::uploadFile(std::string& uploadPath) {
	std::string&	upload_body = request->getBody();
	std::string		boundary = getBoundary();

	if (upload_head.empty()) {
		std::size_t index = upload_body.find("\r\n\r\n");
		if (index != std::string::npos) {
			upload_head.reserve(index + 4);
			upload_head = upload_body.substr(0, index + 4);
			upload_body.erase(0, index + 4);
			uploadPosition = 0;
		}
		std::string filename = getFileName(upload_head);
		if (upload_head.find("filename") != std::string::npos) {
			upload_of.open(uploadPath + (uploadPath[uploadPath.length() - 1] == '/' ? "" : "/") + filename, std::ios::out | std::ios::binary);
		}
		fileSize = upload_body.find("\r\n" + boundary);
	}
	if (fileSize != std::string::npos) {
		size_t i = static_cast<size_t>(upload_of.tellp());
		if (i < fileSize) {
			size_t writeSize = static_cast<size_t>(WRITE_BUFFER_SIZE);
			if ((i + WRITE_BUFFER_SIZE) > fileSize)
				writeSize = fileSize - i;
			std::cout << writeSize << " uploaded\n";
			std::cout.flush();
			upload_of.write(upload_body.c_str() + uploadPosition, writeSize);
			uploadPosition += writeSize;
			upload_of.flush();
			return false;
		} else {
			upload_of.close();
			upload_body.erase(0 ,fileSize + ("\r\n" + boundary).length());
			uploadPosition = 0;
			upload_head.clear();
		}
	}
	if (upload_body == "--\r\n") {
		return true;
	}
	return false;
}

std::string	Response::getBoundary() {
	std::string tmp = "boundary=";
	const std::string& reqConType = request->getValueByKey(REQ_CONTENT_TYPE);
	size_t b = reqConType.find(tmp);
	if (b != reqConType.npos) {
		return ("--" + reqConType.substr(b + tmp.length(), -1));
	}
	throw std::runtime_error(returnError(BAD_REQUEST_STATUS));
}

/* ************************************** */
/*            static functions            */
/* ************************************** */

static void	correctPath(std::string& path) {
	size_t tmp = path.find_last_not_of('/');
	if (tmp != path.length() - 1) {
		path.erase(tmp + 2, -1);
	}
}

static bool	findCgiLocation(Request* request, t_location& location) {
	if (location.path[0] == '.') {
		std::string extension = location.path;
		size_t len = request->getHead()[REQ_PATH].length();
		return (extension == &request->getHead()[REQ_PATH][len - extension.length()]);
	}
	return (false);
}

static std::string	getDefaultErrorPage(int status, long long& contentLength) {
	switch (status) {
		case BAD_REQUEST_STATUS:
			contentLength = 354;
			return DEFAULT_400_STATUS_PAGE;
		case FORBIDDEN_STATUS:
			contentLength = 307;
			return DEFAULT_403_STATUS_PAGE;
		case NOT_FOUND_STATUS:
			contentLength = 300;
			return DEFAULT_404_STATUS_PAGE;
		case METHOD_NOT_ALLOWED_STATUS:
			contentLength = 346;
			return DEFAULT_405_STATUS_PAGE;
		case CONFLICT_STATUS:
			contentLength = 346;
			return DEFAULT_409_STATUS_PAGE;
		case INTERNAL_SERVER_ERROR_STATUS:
			contentLength = 373;
			return DEFAULT_500_STATUS_PAGE;
		case CONTENT_TOO_LARGE_STATUS:
			contentLength = 406;
			return DEFAULT_413_STATUS_PAGE;
		case UNSUPPORTED_MEDIA_TYPE_STATUS:
			contentLength = 333;
			return DEFAULT_415_STATUS_PAGE;
		case NOT_IMPLEMENTED_STATUS:
			contentLength = 339;
			return DEFAULT_501_STATUS_PAGE;
		case GATEWAY_TIMEOUT_STATUS:
			contentLength = 269;
			return DEFAULT_504_STATUS_PAGE;
		case CREATED_STATUS:
			contentLength = 321;
			return DEFAULT_301_STATUS_PAGE;	
		case MOVED_PERMANENTLY_STATUS:
			contentLength = 333;
			return DEFAULT_301_STATUS_PAGE;
		case HTTP_VERSION_NOT_SUP_STATUS:
			contentLength = 306;
			return DEFAULT_505_STATUS_PAGE;
		default:
			contentLength = 0;
			return (std::to_string(status) + " status code not handled");
	}
}

// Helper function to check if a method is allowed in a given list
static bool isMethodAllowed(const std::string& method, const std::vector<std::string>& allowedMethods) {
    return (std::find(allowedMethods.begin(), allowedMethods.end(), method) != allowedMethods.end());
}

/*
	Determines if a given HTTP request method is allowed based on the configuration settings.
	It checks whether the method is included in the allowed methods for the location.
	If the location-specific allowed methods are empty, it checks the server-level allowed methods.
	If the method is allowed in either the location or server configuration, it returns true.
	Otherwise, it returns false, indicating that the method is not allowed.
*/
static bool findAllowedMethod(const std::string& method, const t_server& server, const t_location* location) {
    if (!location) {
        return isMethodAllowed(method, server.allowedMethods);
    } else {
        bool inLocation = isMethodAllowed(method, location->allowedMethods);
        bool inServer = isMethodAllowed(method, server.allowedMethods);
		return (inLocation || (location->allowedMethods.empty() && inServer));
	}
}

static bool	havePermission(const std::string& path, const std::string& uploadPath) {
	size_t found = path.find(uploadPath, 0);
	return ((found != path.npos)
		&& ((found + uploadPath.length() - 1) != (path.length() - 1)));
}

/*
	Generates an HTML representation of the contents of a directory.
	Given a directory pointer `dir` and the `root` path, this function reads
	the directory entries and constructs an HTML list of links to each entry.
	If an entry is a directory, it adds a trailing slash to the link to indicate
	that it's a subdirectory.
*/
static std::string	directory_listing(DIR* dir, std::string root) {
	std::string response;
	response = "<html><body><ul>";

	struct dirent* entry;
	while ((entry = readdir(dir))) {
		if (root[root.length() - 1] != '/') {
			root.insert(root.length(), "/");
		}
		response += "<li><a href=\"" + root + std::string(entry->d_name) +
					(entry->d_type == DT_DIR ? "/\">" : "\">") +
					std::string(entry->d_name) + "</a></li>";
	}
	response += "</ul></body></html>";
	return (response);
}

static std::string extractContentType(const std::string& str) {
	size_t c = str.find(';');
	if (c != str.npos) {
		return (trim(str.substr(0, c)));
	}
	return ("");
}

/* ************************************** */
/*        Response class functions        */
/* ************************************** */

Response::Response(int status, t_config& config) : config(config) {
	cgi = NULL;
	uploaded = true;
	contentLength = 0;
	headerDone = false;
	uploadDone = false;
	request = NULL;
	serverIndex = 0;
	locationIndex = -1;
	fileSize = -1;
	chunked = false;
	initStatusCodeMap();
	try {
		std::cout << RED "Error >> " YELLOW "request: " << status << "\n" PRINT_LINE_AND_FILE;
		returnError(status);
	} catch (std::exception& ex) {}
	// std::cout << "response " << response <<std::endl;
}

Response::Response() {
	chunked = false;
	cgi = NULL;
	std::cout << "Default constructor for response.";
}

Response::Response(Request* request, t_config& config)
	: request(request), config(config), headerDone(false) {
	chunked = false;
	cgi = NULL;
	uploaded = true;
	fileSize = -1;
	uploadPosition = 0;
	contentLength = 0;
	uploadDone = false;
	serverIndex = -1;
	locationIndex = -1;
	defaul_page_flag = false;
	if (request) {
		setContentType(getExtension(request->getValueByKey(REQ_PATH)));
	}
	initStatusCodeMap();
	path = matching();
	correctPath(path); // If the path contains a lot of slashes at the end.
}

Response::~Response() {
	if (currentFile.is_open()) {
		currentFile.close();
		// std::cout << GREEN "file" RED " Closed" RESET_COLOR "\n";
	}
	// std::cout << GREEN "Response deleted!" RESET_COLOR "\n";
	delete cgi;
	cgi = NULL;
	request = NULL;
}

void	Response::defaultPage() {
	t_server server = getServer();
	setContentType(".html");
	body = DEFAULT_PAGE;
	contentLength = body.length();
	setHeader(200);
	response = header + body;
	header.clear();
	defaul_page_flag = true;
	// throw std::runtime_error(response);
}

std::string	Response::matching() {
	serverExists();
	t_server server = getServer();
	std::string	reqPath = request->getValueByKey(REQ_PATH);
	std::string	root = server.root;

	locationExists();
	if (locationIndex == -1) {
		if (root.empty() && server.redirectTo.empty() && reqPath.find_first_not_of('/') == reqPath.npos)
			defaultPage();
		return (root + reqPath);
	}
	t_location	location = getLocation();
	if (!location.root.empty()) {
		root = location.root;
	}
	return (root + reqPath);
}

void	Response::serverExists() {
	std::vector<t_server> servers = config.servers;
	std::string	reqPort	= request->getValueByKey(REQ_PORT).c_str();
	std::string	reqHost = request->getValueByKey(REQ_SERVER_NAME);
	std::string reqIpAddress = getHostInfo(reqHost.c_str(), reqPort.c_str());

	for (size_t serverIndex = 0; serverIndex < servers.size(); serverIndex++) {
		std::string	configPort	= to_string(servers[serverIndex].port);
		std::string	configHost = servers[serverIndex].serverName;
		if ((reqIpAddress == getHostInfo(configHost.c_str(), configPort.c_str())/*servers[serverIndex].serverName*/)
			&& atoi(reqPort.c_str()) == servers[serverIndex].port) {
			this->serverIndex = serverIndex;
			return ;
		}
	}
	this->serverIndex = 0; // If no mat.ching server is found, the first server will be set as the default one.
}

void	Response::locationExists() {
	std::vector<t_location>		locations = getServer().locations;
	std::vector<std::string>	locationPaths;

	for (size_t i = 0; i < locations.size(); i++) {
		// here where the CGI location handled!
		if (findCgiLocation(request, locations[i])) {
			std::string	cgiPath = request->getValueByKey(REQ_PATH);
			cgiPath.insert(0, config.servers[serverIndex].locations[i].root);
			if (access(cgiPath.c_str(), F_OK) != -1) {
				locationIndex = i;
				config.servers[serverIndex].locations[i].isCgi = true;
				return ;
			}
			/// CHECKLATER: in this case i found the cgi location but the file requested didn't work
			locationIndex = -1; // already contains -1
			return ;
		}
		config.servers[serverIndex].locations[i].isCgi = false;
		locationPaths.push_back(locations[i].path);
	}
	std::string lookFor = request->getValueByKey(REQ_PATH);
	while (lookFor.size() > 0) {
		std::vector<std::string>::iterator it = std::find(locationPaths.begin(), locationPaths.end(), lookFor);
		if (it != locationPaths.end()) {
			locationIndex = std::distance(locationPaths.begin(), it);
			return ;
		}
		size_t	last = lookFor.find_last_of('/');
		if (last == lookFor.npos)
			break ;
		lookFor.erase(last, -1);
	}
	if (request->getValueByKey(REQ_PATH)[0] == '/' && lookFor.empty()) {
		std::vector<std::string>::iterator it = std::find(locationPaths.begin(), locationPaths.end(), "/");
		if (it != locationPaths.end()) {
			locationIndex = std::distance(locationPaths.begin(), it);
			return ;
		}
	}
}

void	Response::methodNotAllowed() {
	t_server			server = getServer();
	t_location			*location = NULL;
	const std::string&	method = request->getValueByKey(REQ_METHOD);

	if (locationIndex > -1) {
		location = &server.locations[locationIndex];
	}

	if (method != "GET" && method != "POST" && method != "DELETE") {
		throw std::runtime_error(returnError(NOT_IMPLEMENTED_STATUS));
	} else if (findAllowedMethod(method, server, location) == false) {
		throw std::runtime_error(returnError(METHOD_NOT_ALLOWED_STATUS));
	}
}

void	Response::locationRedirection() { // add a body!
	t_server	server = getServer();
	std::string redirTo;
	int			redirCode;

	if (!server.redirectTo.empty()) {
		redirTo = server.redirectTo;
		redirCode = server.redirectionCode;
	} else {
		t_location	location = getLocation();
		redirTo = location.redirectTo;
		redirCode = location.redirectionCode;
	}
	if (!headerDone) {
		header = "HTTP/1.1";
		header += this->statusCode[redirCode] + "\r\n";
		header += "Content-Length: 0\r\n";
		if (request)
			header += "Set-Cookie: " + request->getValueByKey(REQ_COOKIES) + "\r\n";
		header += "Location: " + redirTo;
		header += "\r\n\r\n";
		headerDone = true;
	}
	response = header;
	header.clear();
}

bool	Response::deleteRequested() {
	std::string cmd = "/bin/rm -rf " + path;
	return (!std::system(cmd.c_str()));
	// std::cout << path << " <----------------Deleted!----------------\n";
	// return (true);
}

void	Response::deleteMethod(DIR* dir) {
	std::string cgiExecutable;
	std::string index;
	std::string uploadPath;

	if (locationIndex > -1) {
		t_location location = getLocation();
		index = location.index;
		cgiExecutable = location.cgiExecutable;
		uploadPath = location.uploadPath;
	} else {
		t_server server = getServer();
		index = server.index;
		cgiExecutable = server.cgiExecutable;
		uploadPath = server.uploadPath;
	}

	if (access(path.c_str(), F_OK) < 0) {
		throw std::runtime_error(returnError(NOT_FOUND_STATUS));
	}

	if (havePermission(path, uploadPath)) {
		if (locationIndex > -1 && getLocation().isCgi == true) {
			return (executeCgi(path, cgiExecutable));
		}
		if (dir) {
			if (path[path.length() - 1] != '/') {
				throw std::runtime_error(returnError(CONFLICT_STATUS));
			}
			if (!index.empty() && !cgiExecutable.empty()) {
				std::cout << "delete CGI\n";
				return (executeCgi(path + index, cgiExecutable));
			}
			if (deleteRequested() == false) {
				throw std::runtime_error(returnError(INTERNAL_SERVER_ERROR_STATUS));
			}
			contentLength = 0;
			
			setHeader(NO_CONTENT_STATUS);
			response = header;
			header.clear();
			return ;
		} else {
			if (!cgiExecutable.empty()) {
				return (executeCgi(path, cgiExecutable));
			}
			if (deleteRequested() == false) {
				throw std::runtime_error(returnError(INTERNAL_SERVER_ERROR_STATUS));
			}
			contentLength = 0;
			
			setHeader(NO_CONTENT_STATUS);
			response = header;
			header.clear();
			return ;
		}
	}
	throw std::runtime_error(returnError(FORBIDDEN_STATUS));
}

bool	Response::redirectDir() { // add a body!
	std::string request_path = request->getValueByKey(REQ_PATH);
	if (request_path.length() > 1 && request_path[request_path.length() - 1] != '/') {
		if (!headerDone) {
			header = "HTTP/1.1";
			header += this->statusCode[MOVED_PERMANENTLY_STATUS] + "\r\n";
			header += "Content-Length: 0\r\n";
			if (request)
				header += "Set-Cookie: " + request->getValueByKey(REQ_COOKIES) + "\r\n";
			header += "Location: " + request_path + "/";
			header += "\r\n\r\n";
			headerDone = true;
		}
		response = header;
		return (true);
	}
	return (false);
}

void	Response::listDirectory(DIR* dir) {
	std::string	index;
	std::string	cgiExecutable;
	bool		autoIndex;

	if (locationIndex > -1) {
		index = getLocation().index;
		autoIndex = getLocation().autoindex;
		cgiExecutable = getLocation().cgiExecutable;
	} else {
		index = getServer().index;
		autoIndex = getServer().autoindex;
		cgiExecutable = getServer().cgiExecutable;
	}

	if (redirectDir()) return ;
	std::string indexFile = path + (path[path.length() - 1] != '/' ? "/" : "") + "index.html";
	if (index.empty()) {
		try {
			setContentType(".html");
			servFile(indexFile, 200, 0);
			return ;
		} catch(const std::exception& e) {/* if not found keep going */}
	} else if (!index.empty()) {
		if (index[0] == '/') {
			throw std::runtime_error(returnError(NOT_FOUND_STATUS));
		}
		indexFile = path + index; /* the path alawys will contain '/' at the end [redirectDir()] */
		if (!cgiExecutable.empty()) {
			return (executeCgi(indexFile, cgiExecutable));
		} else {
			setContentType(getExtension(indexFile));
			return (servFile(indexFile, 200, NOT_FOUND_STATUS)); // if is an exist folder i response with NOT_FOUND and ngnix does not
		}
	}
	if (autoIndex == true) {
		body = directory_listing(dir, request->getValueByKey(REQ_PATH));
		contentLength = body.length();	
		setContentType(".html");
		
		setHeader(200);
		response = header + body;
		header.clear();
		return ;
	}
	throw std::runtime_error(returnError(FORBIDDEN_STATUS));
}

std::string	Response::fileToString(const std::string& fileName, int errorStatus) {
	if (!currentFile.is_open()) {
		currentFile.open(fileName);
		struct stat st;
		if (stat(fileName.c_str(), &st) == 0)
			contentLength = st.st_size;
	}

	if (!currentFile.is_open() || isDirectory(fileName.c_str())) {
		throw std::runtime_error(getDefaultErrorPage(errorStatus, contentLength));
	}

	char buffer[BUFFER_SIZE + 1];
	bzero(buffer, BUFFER_SIZE + 1);
	currentFile.read(buffer, BUFFER_SIZE);
	std::string result(buffer, currentFile.gcount());
	return (result);
}

void	Response::servFile(std::string& filePath, int status, int errorStatus) {
	try {
		body = fileToString(filePath, errorStatus);
		setHeader(status);
		response = header + body;
		header.clear();
		// std::cout << " " RED << this->mimeType << RESET_COLOR "\n";

	} catch(std::exception& ex) {
		if (errorStatus == 0)
			throw ;
		throw std::runtime_error(returnError(NOT_FOUND_STATUS));
	}
}

void	Response::postMethod(DIR* dir, const std::string& path) {
	std::string cgiExecutable;
	std::string index;
	std::string uploadPath;
	size_t	clientMaxBodySize = 0;
	size_t	req_content_length = request->getContentLength();

	if (locationIndex > -1) {
		cgiExecutable = getLocation().cgiExecutable;
		index = getLocation().index;
		uploadPath = getLocation().uploadPath;
		clientMaxBodySize = getLocation().clientMaxBodySize ;
		if (clientMaxBodySize == 0)
			clientMaxBodySize = getServer().clientMaxBodySize;
	} else {
		cgiExecutable = getServer().cgiExecutable;
		index = getServer().index;
		uploadPath = getServer().uploadPath;
		clientMaxBodySize = getServer().clientMaxBodySize;
	}

	if (req_content_length > clientMaxBodySize) {
		std::cout << "config " << clientMaxBodySize << " request "  << req_content_length << std::endl;
		throw std::runtime_error(returnError(CONTENT_TOO_LARGE_STATUS)); //set the macro
	}
	if (locationIndex > -1 && getLocation().isCgi == true) {
		return (executeCgi(path, cgiExecutable));
	}
	if (!uploadPath.empty()) {
		// std::cout << request->getValueByKey(REQ_CONTENT_TYPE) << "<---------------\n";
		if (extractContentType(request->getValueByKey(REQ_CONTENT_TYPE)) != "multipart/form-data") { // request contens no content type
			throw std::runtime_error(returnError(UNSUPPORTED_MEDIA_TYPE_STATUS));
		}
		DIR *uploadDir = opendir(uploadPath.c_str());
		if (!uploadDir) {
			throw std::runtime_error(returnError(NOT_FOUND_STATUS));
		}
		closedir(uploadDir);
		if (uploadFile(uploadPath)) {
			std::string body = UPLOADED_DEFAULT_PAGE;
			contentLength = body.length();
			setContentType(".html");
			setHeader(200);
			response = header + body;
			uploaded = true;
			return ;
		}
		uploaded = false;
		return ;
	} else {
		if (access(path.c_str(), F_OK) < 0) {
			throw std::runtime_error(returnError(NOT_FOUND_STATUS));
		}
		if (dir) {
			if (redirectDir()) {return ;}
			if (!index.empty() && !cgiExecutable.empty()) {
				return (executeCgi(path + index, cgiExecutable));
			}
		} else {
			if (!cgiExecutable.empty()) {
				return (executeCgi(path, cgiExecutable));
			}
		}
	}
	throw std::runtime_error(returnError(FORBIDDEN_STATUS));
}

const std::string&	Response::returnError(int status) {
	if (serverIndex < 0) {
		// Handle the case when the server index is not set.
		std::cout << GREEN "Server index is not set\n" PRINT_LINE_AND_FILE;
		setContentType(".html");
		body = "Server index is not set.";
		contentLength = body.length();
		setHeader(status);
	} else {
		t_server server = getServer();

		if (locationIndex >= 0) {
			// Handle the case when the location index is set.
			t_location location = getLocation();
			try {
				body = fileToString(location.errorPages[status], status);
			} catch (const std::exception& e) {
				// Use the server's error pages if the location's error pages are not found.
				try {
					body = fileToString(server.errorPages[status], status);
				} catch (const std::exception& ex) {
					body = ex.what();
					contentLength = body.length();
				}
			}
		} else {
			// Handle the case when the location index is not set.
			try {
				body = fileToString(server.errorPages[status], status);
			} catch (const std::exception& ex) {
				body = ex.what();
				contentLength = body.length();
			}
		}
		setContentType(".html");
		setHeader(status);
		response = header + body;
		header.clear();
	}
	return (response);
}

std::string	Response::buildResponse( void ) {
	if (defaul_page_flag == true) {return (response);}
	DIR *dir = opendir(path.c_str());
	try {
		std::string requestMethod = request->getValueByKey(REQ_METHOD);

		methodNotAllowed();
		if (!getServer().redirectTo.empty()
			|| (locationIndex > -1 && !getLocation().redirectTo.empty())) {
			locationRedirection();
		} else if (requestMethod == "DELETE") {
			deleteMethod(dir);
		} else if (requestMethod == "POST") {
			postMethod(dir, path);
		} else if (dir) {
			listDirectory(dir);
		} else if (locationIndex > -1 && getLocation().isCgi) {
			executeCgi(path, getLocation().cgiExecutable);
		} else {
			servFile(path, 200, NOT_FOUND_STATUS);
		}
	}
	catch(const std::exception& e) {}
	if (dir)
		closedir(dir);
	return (response);
}

/* ******************************* */
/*             Getters             */
/* ******************************* */

const t_server&	Response::getServer() {
	if (serverIndex < 0)
		throw std::out_of_range("getServer()");
	return (config.servers[serverIndex]);
}

const t_location&	Response::getLocation() {
	if (locationIndex < 0 || serverIndex < 0)
		throw std::out_of_range("getLocation()");
	return (getServer().locations[locationIndex]);
}

const std::string&	Response::getResponse() {
	return (response);
}

const Request*	Response::getRequest() {
	return (request);
}

bool Response::getUploaded() {
	return (uploaded);
}

/* ******************************* */
/*             Setters             */
/* ******************************* */

void	Response::initStatusCodeMap() {
	statusCode[200] = " 200 OK";
	statusCode[201] = " 201 Created";
	statusCode[204] = " 204 No Content";
	statusCode[301] = " 301 Moved Permanently";
	statusCode[403] = " 403 Forbidden";
	statusCode[404] = " 404 Not Found";
	statusCode[405] = " 405 Method Not Allowed";
	statusCode[501] = " 501 Not Implemented";
	statusCode[304] = " 304 Not Modified";
	statusCode[400] = " 400 Bad Request";
	statusCode[409] = " 409 Conflict";
	statusCode[204] = " 204 No Content";
	statusCode[500] = " 500 Internal Server Error";
	statusCode[413] = " 413 Content Too Large";
	statusCode[415] = " 415 Unsupported Media Type";
	statusCode[504] = " 504 Gateway Timeout";
	statusCode[505] = " 505 HTTP Version Not Supported";
}

void	Response::setContentType(const std::string& extension) {
	static std::map<std::string, std::string> contentTypeMap;
	static int i = 0;

	if (!i) {
		contentTypeMap[".aac"]		= "audio/aac";
		contentTypeMap[".abw"]		= "application/x-abiword";
		contentTypeMap[".arc"]		= "application/x-freearc";
		contentTypeMap[".avif"]		= "image/avif";
		contentTypeMap[".avi"]		= "video/x-msvideo";
		contentTypeMap[".azw"]		= "application/vnd.amazon.ebook";
		contentTypeMap[".bin"]		= "application/octet-stream";
		contentTypeMap[".bmp"]		= "image/bmp";
		contentTypeMap[".bz"]		= "application/x-bzip";
		contentTypeMap[".bz2"]		= "application/x-bzip2";
		contentTypeMap[".cda"]		= "application/x-cdf";
		contentTypeMap[".csh"]		= "application/x-csh";
		contentTypeMap[".css"]		= "text/css";
		contentTypeMap[".csv"]		= "text/csv";
		contentTypeMap[".doc"]		= "application/msword";
		contentTypeMap[".docx"]		= "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
		contentTypeMap[".eot"]		= "application/vnd.ms-fontobject";
		contentTypeMap[".epub"]		= "application/epub+zip";
		contentTypeMap[".gz"]		= "application/gzip";
		contentTypeMap[".gif"]		= "image/gif";
		contentTypeMap[".htm"]		= "text/html";
		contentTypeMap[".html"]		= "text/html";
		contentTypeMap[".ico"]		= "image/vnd.microsoft.icon";
		contentTypeMap[".ics"]		= "text/calendar";
		contentTypeMap[".jar"]		= "application/java-archive";
		contentTypeMap[".jpeg"]		= "image/jpeg";
		contentTypeMap[".jpg"]		= "image/jpeg";
		contentTypeMap[".js"]		= "text/javascript";
		contentTypeMap[".json"]		= "application/json";
		contentTypeMap[".jsonld"]	= "application/ld+json";
		contentTypeMap[".midi"]		= "audio/midi";
		contentTypeMap[".mid"]		= "audio/x-midi";
		contentTypeMap[".mjs"]		= "text/javascript";
		contentTypeMap[".mp3"]		= "audio/mpeg";
		contentTypeMap[".mp4"]		= "video/mp4";
		contentTypeMap[".mpeg"]		= "video/mpeg";
		contentTypeMap[".mpkg"]		= "application/vnd.apple.installer+xml";
		contentTypeMap[".odp"]		= "application/vnd.oasis.opendocument.presentation";
		contentTypeMap[".ods"]		= "application/vnd.oasis.opendocument.spreadsheet";
		contentTypeMap[".odt"]		= "application/vnd.oasis.opendocument.text";
		contentTypeMap[".oga"]		= "audio/ogg";
		contentTypeMap[".ogv"]		= "video/ogg";
		contentTypeMap[".ogx"]		= "application/ogg";
		contentTypeMap[".opus"]		= "audio/opus";
		contentTypeMap[".otf"]		= "font/otf";
		contentTypeMap[".png"]		= "image/png";
		contentTypeMap[".pdf"]		= "application/pdf";
		contentTypeMap[".php"]		= "application/x-httpd-php";
		contentTypeMap[".ppt"]		= "application/vnd.ms-powerpoint";
		contentTypeMap[".pptx"]		= "application/vnd.openxmlformats-officedocument.presentationml.presentation";
		contentTypeMap[".rar"]		= "application/vnd.rar";
		contentTypeMap[".rtf"]		= "application/rtf";
		contentTypeMap[".sh"]		= "application/x-sh";
		contentTypeMap[".svg"] 		= "image/svg+xml";
		contentTypeMap[".tar"] 		= "application/x-tar";
		contentTypeMap[".tif"] 		= "image/tiff";
		contentTypeMap[".tiff"]		= "image/tiff";
		contentTypeMap[".ts"]		= "video/mp2t";
		contentTypeMap[".ttf"]		= "font/ttf";
		contentTypeMap[".txt"]		= "text/plain";
		contentTypeMap[".vsd"]		= "application/vnd.visio";
		contentTypeMap[".wav"]		= "audio/wav";
		contentTypeMap[".weba"]		= "audio/webm";
		contentTypeMap[".webm"]		= "video/webm";
		contentTypeMap[".webp"]		= "image/webp";
		contentTypeMap[".woff"]		= "font/woff";
		contentTypeMap[".woff2"]	= "font/woff2";
		contentTypeMap[".xhtml"]	= "application/xhtml+xml";
		contentTypeMap[".xls"]		= "application/vnd.ms-excel";
		contentTypeMap[".xlsx"]		= "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
		contentTypeMap[".xml"]		= "application/xml";
		contentTypeMap[".xul"]		= "application/vnd.mozilla.xul+xml";
		contentTypeMap[".zip"]		= "application/zip";
		contentTypeMap[".3gp"]		= "video/3gpp";
		contentTypeMap[".3g2"]		= "video/3gpp2";
		contentTypeMap[".7z"]		= "application/x-7z-compressed";
		contentTypeMap[".ico"]		= "image/x-icon";
		i = 1;
	}
	std::string tmp = contentTypeMap[extension];
	if (tmp.empty()) {
		tmp = "application/octet-stream";
		// tmp = "text/plain";
	}
	this->mimeType = tmp;
}

void	Response::setHeader(int status) {
	if (!headerDone) {
		header = "HTTP/1.1";
		header += this->statusCode[status] + "\r\n";
		header += "Connection: keep-alive\r\n";
		header += "Accept-Ranges: bytes\r\n";
		header += "Content-type: " + this->mimeType + "\r\n";
		header += "Content-length: " + to_string(contentLength) + "\r\n";
		if (request)
			header += "Set-Cookie: " + request->getValueByKey(REQ_COOKIES) + "\r\n";
		header += "\r\n";
		headerDone = true;
	}
}
