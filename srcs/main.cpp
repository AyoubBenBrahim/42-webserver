
#include "parse_configFile.hpp"
#include "Server.hpp"

#define TEST_CONFIG_FILE

#ifdef TEST_CONFIG_FILE
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

    for (size_t i = 0; i < http_servers.size(); ++i)
    {
        std::cout << "=================Server[" << i + 1 << "]====================" << std::endl;

        std::cout << "port         : " << http_servers[i].port << std::endl;
        std::cout << "host         : " << inet_ntoa(http_servers[i].host) << std::endl;
        std::cout << "server       : " << http_servers[i].server_name << std::endl;
        std::cout << "root         : " << http_servers[i].root << std::endl;
        std::cout << "AUTOINDEX    : " << http_servers[i].autoindex << std::endl;
        std::cout << "upload_path  : " << http_servers[i].upload_path << std::endl;

        for (size_t j = 0; j < http_servers[i].index_vec.size(); ++j)
            std::cout << "indexs[" << j << "] : " << http_servers[i].index_vec[j] << std::endl;

        std::cout << "bodySize     : " << http_servers[i].client_max_body_size << std::endl;

        for (size_t j = 0; j < http_servers[i].vec_of_locations.size(); ++j)
        {
            std::cout << "\n  ==LOCATION=== \n";
            std::map<std::string, std::string>::const_iterator map_iter;
            for (map_iter = http_servers[i].vec_of_locations[j].begin(); map_iter != http_servers[i].vec_of_locations[j].end(); ++map_iter)
            {
                std::cout << map_iter->first << " : " << map_iter->second << std::endl;
            }
        }

        std::cout << "\n  ==ERROR PAGE=== \n";
        std::map<int, std::string>::const_iterator map_iter;
        for (map_iter = http_servers[i].map_of_error_page.begin(); map_iter != http_servers[i].map_of_error_page.end(); ++map_iter)
        {
            std::cout << map_iter->first << " : " << map_iter->second << std::endl;
        }
    }

   

    
    return 0;
}
#endif

// int main()
// {
//     Server server = Server();
//     server.run();
// }




// std::vector<HttpServer> _http_servers;
// std::vector<AcceptorSockets> acceptor_sockets;
// std::vector<Client> _clients;
// std::vector<Sockets> _socks;
