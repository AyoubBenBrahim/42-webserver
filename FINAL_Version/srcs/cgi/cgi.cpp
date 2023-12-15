
#include <Cgi.hpp>
#include <macros.hpp>

void convertToUpperCaseUnderscore(std::string &input) {
	std::string result;
	for (size_t i = 0; i < input.size(); i++) {
		if (input[i] == '-') {
			input[i] = '_';
		}
		else if (input[i] != '_' && std::isalpha(input[i]) && std::islower(input[i])) {
			input[i] -= 32;
		}
	}
}

static char	**buildEnv(Request* request) {
	char	**env;
	std::map<std::string, std::string>				&tmp = request->getHead();
	std::map<std::string, std::string>::iterator	it;
	int												i;

	i = 0;

	env = new char*[tmp.size() + 1];
	for (it = tmp.begin(); it != tmp.end(); it++) {
		std::string key = it->first;
		if (key != REQ_CONTENT_LENGTH && key != REQ_QUERY_STRING && key != REQ_CONTENT_TYPE && key != REQ_METHOD)
			key.insert(0, "HTTP_");
		convertToUpperCaseUnderscore(key);
		env[i++] = strdup((key + "=" + it->second).c_str());
	}
	env[i] = NULL;
	return (env);
}


Cgi::~Cgi() {
	close(in);
	close(out);
	kill(pid, SIGTERM);
	delete[] env;
}

Cgi::Cgi(Request* request, const std::string& cgiExecutable, const std::string& scriptPath) {
	env = buildEnv(request);
	program[0] = (char *)cgiExecutable.c_str();
	program[1] = (char *)scriptPath.c_str();
	program[2] = NULL;
}

char **Cgi::getEnv() {
	return env;
}

char **Cgi::getProgram() {
	return program;
}

void Cgi::setPid(int pid) {
	this->pid = pid;
}

int Cgi::getPid() {
	return pid;
}

void Cgi::setIn(int in) {
	this->in = in;
}

int Cgi::getIn() {
	return in;
}


void Cgi::setOut(int out) {
	this->out = out;
}

int Cgi::getOut() {
	return out;
}


int Cgi::getTimeout() {
	return (timeout);
}