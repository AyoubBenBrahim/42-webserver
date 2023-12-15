#include <Parsing.hpp>
#include <Utils.hpp>
#include <macros.hpp>
/** @brief
	This parsing based on these pillars
	1- HEADER
		/r/n/r/n
		BODY
	2 - The first line contains the METHOD PATH HTTPVERSION
	and the second one containg the HOST PORT
	and the rest would look like Key: Value
*/
 

void	parseStartLine(std::map<std::string, std::string> &headMap, std::string line) {
	std::vector<std::string>	firstLine;
	std::vector<std::string>	splittedPath;
	
	std::istringstream iss1(line);
	while (std::getline(iss1, line, ' ')) {
		if (!line.empty()) 
			firstLine.push_back(line);
	}
	if (firstLine.size() == 3) {
		headMap[REQ_METHOD]  = trim(firstLine[0]);
		if (firstLine[1].find("?") != std::string::npos) {
			splittedPath = splitLine(firstLine[1], "?");
			headMap[REQ_PATH] = splittedPath[0];
			if (splittedPath.size() > 1)
				headMap[REQ_QUERY_STRING] = splittedPath[1];
		}
		else
			headMap[REQ_PATH]    = trim(firstLine[1]);
		headMap[REQ_HTTP_VERSION] = trim(firstLine[2]);
		if (headMap[REQ_HTTP_VERSION] != "HTTP/1.1")
			throw (HTTP_VERSION_NOT_SUP_STATUS);
	} else {
		throw (BAD_REQUEST_STATUS);
	}
}

// TODO: remove the (size_t i) param

void	setKeyAndValue(std::string line, std::map<std::string, std::string>& headMap) {
	std::string key, value;

	std::istringstream iss(line);
	char delimiter;
	if (line.find(":") != std::string::npos) {
		delimiter = ':';
	}
	else
		throw (BAD_REQUEST_STATUS);
	std::getline(iss, key, delimiter);
	key = trim(key);
	value = &line[key.length() + 2];
	if (key.empty() || value.empty())
		return ;

	if (value[0] == '\"' && value[value.length() - 1] == '"')
		value = value.substr(1, value.length() - 2);
	if (key == "Host" && !headMap[key].empty()) 
		throw (BAD_REQUEST_STATUS);
	headMap[key] = value;
	if (key == "Host") {
		value = headMap[key];//localhost:8080
		headMap.erase(key);
		key = REQ_SERVER_NAME;
		std::istringstream iss(value);
		std::string servername, port;
		std::getline(iss, servername, delimiter);
		std::getline(iss, port, delimiter);
		headMap[REQ_SERVER_NAME] = servername;
		if (!port.empty()) {
			headMap[REQ_PORT] = port;
		}
	}
	else 
	{
		headMap[key] = value;
	}
}

void	 parseHead(std::map<std::string, std::string>& headMap, std::vector<std::string> lines) {
	std::string					key;
	std::string					value;

	for (size_t i = 1; i < lines.size(); i++) {
		setKeyAndValue(lines[i], headMap);
		// if (lines[i].find(";") != std::string::npos && lines[i].find(REQ_BOUNDARY) == std::string::npos)
		// 	valuesWithinLine = splitLine(lines[i], ";");
		// else 
		// 	valuesWithinLine.push_back(lines[i]);
		// for (size_t j = 0; j < valuesWithinLine.size(); j++) {
		// 	try {
		// 	} catch(std::exception &e) {
		// 		e.what();
		// 	}
		// }
	}
}

/** @brief this function specify the body type ,and init the request map 
 * with appropriate elements [if we have a boundary it would be assigned ..]
 * if the type is none then we don't need to parse the request
*/


// R`

static void removeSpaces(std::string &buffer, size_t index) {
	size_t		start;

	start = index;
	while (start < buffer.length() && buffer[start] == ' ')
		start++;
	if (start > index + 1)
		buffer.erase(index, start - index - 1);
}

static void	checkEnclosed(char c, std::stack<char> &myStack) {
	if (myStack.empty() || myStack.top() != c)
		myStack.push(c);
	else
		myStack.pop();
}

static void	checkLine(std::string& line, std::stack<char>& myStack, bool& quotes) {
	for (size_t j = 0; j < line.size(); j++) {
		if (line[j] == '\"' || line[j] == '\'')
		{
			quotes = !quotes;
			checkEnclosed(line[j], myStack);
		}
		else if (line[j] == ' ' && !quotes)  // remove extra spaces
			removeSpaces(line, j);
	}
}

void	checkRequest(std::string	buffer) {
	// std::string correctRequest;
	std::stack<char>		myStack;
	std::string 			head;
	bool					quotes;

	quotes = false;
	size_t last = buffer.size() - 1;
	if (buffer[last] != '\n' || last - 1 < 0 || buffer[last - 1] != '\r')
		throw (BAD_REQUEST_STATUS);
	std::vector<std::string> lines = splitLine(buffer, "\r\n");
	// buffer = "";
	for (size_t i = 0; i < lines.size(); i++) {
		checkLine(lines[i], myStack, quotes);
		// buffer += (lines[i] + "\r\n");
	}
	if (myStack.size()) {
		// std::cout << buffer.substr(buffer.length() - 20, buffer.length()) << std::endl;
		throw (BAD_REQUEST_STATUS);
	}
}

void	checkPath(std::string path, std::string connection) {
	if (path.length() > 2048) {
		if (connection == "keep-alive")
			throw (1);
		throw (0);
	}
	if (path.find_first_not_of(ALLOWED_URI_CHARS) != std::string::npos)
		throw (BAD_REQUEST_STATUS);
}

/** @brief this parse does not check if the request is bad formated
 * //TODO: look for when a request is bad;
*/



std::string getUnchunkedContent(std::string content) {
	std::vector<std::string> lines;
	std::string result;
	size_t		chunkSize;

	lines = splitLine(content, "\r\n");
	for (size_t j = 0; j < lines.size(); j++) {
		chunkSize = std::atoll(lines[j++].c_str());
		while (chunkSize > 0) {
            if (lines[j].length() > chunkSize)
                result += lines[j].substr(0, chunkSize);
            else {
                result += lines[j];
                break ;
            }
            chunkSize -= lines[j].size();
		}
	}
	return (result + "\r\n");
}

void	unchunkBody(std::string& body, std::string boundary) {
	std::string content, tmp;
    size_t s;
    size_t e;

	if (!boundary.empty()) {
        s = boundary.length();
        while (s < body.length()) {
            tmp = &body[s];
            // size_t index = s;
            if (tmp.find(boundary) == std::string::npos)
                e = tmp.length() + s;
            else
                e = tmp.find(boundary) + s;
            if (tmp.find("\r\n\r\n") == std::string::npos)
                return ;
            s = tmp.find("\r\n\r\n") + 4 + s;
            content = body.substr(s, e - s);
            content = getUnchunkedContent(content);
            body.erase(s, e - s);
            body.insert(s, content);
            s += content.length() + boundary.length();
        }
    }
    else 
        body = getUnchunkedContent(body);
}
