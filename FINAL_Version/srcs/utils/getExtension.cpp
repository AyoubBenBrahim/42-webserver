#include <Utils.hpp>

const std::string	getExtension(const std::string& path) {
	std::string extension;
	size_t dot = path.find_last_of('.');
	if (dot != path.npos) {
		extension = path.substr(dot, path.length());
	}
	return (extension);
}
