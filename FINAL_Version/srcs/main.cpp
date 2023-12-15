
#include "Server.hpp"
#include "AcceptorSockets.hpp"

static void	usage(const char* programName) {
	std::cerr << GREEN "Usage: " RED << programName << " [config_file_path]" << RESET_COLOR << std::endl;
	std::cerr << PRINT_LINE_AND_FILE;
	exit(1);
}

int main(int argc, char **argv)
{
	argv[1] = (argc == 2) ? argv[1] : (char*)DEFAULT_CONFIG_FILE;
	
	if (argc > 2) {
		usage(argv[0]);
		return (1);
	}
	t_config config = parseConFile(argv[1]);
	Server server(config);
	server.runServer();
		
	return 0;
} 