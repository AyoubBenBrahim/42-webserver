#include <Utils.hpp>

std::string getHostInfo(const char* host, const char *port) {
	// Set up the hints structure
	struct addrinfo hints;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; // Specify IPv4 only
	hints.ai_socktype = SOCK_STREAM; // Specify socket type (e.g., SOCK_STREAM for TCP)

	// Call getaddrinfo
	struct addrinfo *result, *p;
	int status = getaddrinfo(host, port, &hints, &result);
	if (status != 0) {
		std::cerr << "host: " << host << " port: " << port << std::endl;
		std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
		return ("");
		// return 1;
	}

	// Iterate through the addresses and print information
	char ipstr[INET_ADDRSTRLEN];
	for (p = result; p != nullptr; p = p->ai_next) {
		// Ensure the address is IPv4
		if (p->ai_family == AF_INET) {
			struct sockaddr_in* ipv4 = reinterpret_cast<struct sockaddr_in*>(p->ai_addr);
			// Convert the IPv4 address to a string
			inet_ntop(AF_INET, &(ipv4->sin_addr), ipstr, sizeof(ipstr));
			break ;
		}
	}

	// Free the linked list when done
	freeaddrinfo(result);
	return (ipstr);
}
