
#include "ParseConfigFile.hpp"
#include "Server.hpp"
#include "AcceptorSockets.hpp"


int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: ./webserv <config_file>" << std::endl;
        return 1;
    }

    std::vector<ServerSettings> http_servers;
    try
    {
        parseConfigFile(argv[1], http_servers);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    Server server(http_servers);
    server.printServerSettings();
    server.runServer();
    
    return 0;
}