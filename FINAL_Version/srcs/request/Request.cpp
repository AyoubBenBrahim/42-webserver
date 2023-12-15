#include <Request.hpp>

Request::Request(){
	headEnd = false;
	// outputFile.open((this->bodyFile).c_str());
	// if (!outputFile.is_open())
	// 	std::cout << GREEN " Not Opened\n" RESET_COLOR;
	total = 0;
}

Request::~Request() {
	outputFile.close();
	unlink(bodyFile.c_str());
}

void	Request::setHead( std::map<std::string, std::string>  head) {
	this->head = head;
}

size_t	Request::getContentLength() {
	return (std::atoll(head[REQ_CONTENT_LENGTH].c_str()));
}

void	Request::clear() {
	// outputFile.close();
	headEnd = false;
	headstr.clear();
	head.clear();
	total  = 0;
	rest.clear();
	uploadHead.clear();
}

std::map<std::string, std::string>&	Request::getHead() {
	return this->head;
}

void	Request::addToHead(std::string key, std::string value) {
	this->head[key] = value;
}


std::string&	Request::getValueByKey(std::string key) {
	return (this->head[key]);
}

void	Request::appendHead(char *buffer, int len) {
	std::string tmp;

	this->headstr.append(buffer, len);
	size_t index = this->headstr.find("\r\n\r\n");
	if (index != std::string::npos) {
		this->headEnd = true;
		body = headstr.substr(index + 4, headstr.length() - index);
		total += body.length();
		// outputFile << tmp;
		// outputFile.flush();
		headstr = headstr.substr(0, index + 4);
		
		checkRequest(headstr);
		std::vector<std::string> lines = splitLine(headstr, "\r\n");
		if (lines.size() > 0)
			parseStartLine(head, lines[0]);
		parseHead(head, lines);
		checkPath(head[REQ_PATH], head[REQ_CONNECTION]);
		if (head[REQ_METHOD] == "POST") {
			bool contentLength = !head[REQ_CONTENT_LENGTH].empty();
			bool transfer = !head[REQ_TRANSFER].empty();
			if ((!contentLength && !transfer))
				throw (BAD_REQUEST_STATUS);
			if ((contentLength && transfer))
				throw (BAD_REQUEST_STATUS);
			if (transfer && head[REQ_TRANSFER] != "chunked")
				throw (NOT_IMPLEMENTED_STATUS);
		}
	}
}

void	Request::setBody(std::string body) {
	this->body = body;
}

void	Request::appendFile(char* buffer, int len) {
	total += len;
	std::cout << "total: " << total << " contentLength: " << head[REQ_CONTENT_LENGTH] << std::endl;
	// body += buffer;
	body.append(buffer, len);
	// outputFile.write(buffer, len);
}

void	Request::closeOutFile() {
	outputFile.close();
}

bool	Request::getHeadEnd() const {
	return (this->headEnd);
}

const	std::string	&Request::getHeadStr() const {
	return (this->headstr);
}


const	std::string						&Request::getUploadHead() const {
	return (uploadHead);
}
void									Request::setUploadHead(std::string uploadHead) {
	this->uploadHead = uploadHead;
}

const	std::string	&Request::getRest() const {
	return (rest);
}

// const	std::string	&Request::getBodyFile() const {
// 	return (bodyFile);
// }

const	size_t	&Request::getTotal() const {
	return (total);
}

void	Request::setRest(std::string rest) {
	this->rest = rest;
}

std::string& Request::getBody() {
	return (body);
}