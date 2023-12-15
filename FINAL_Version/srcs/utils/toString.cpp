#include <Utils.hpp>

std::string	to_string(long long num) {
	std::stringstream	ss;

    ss << num;
	return (ss.str());
}