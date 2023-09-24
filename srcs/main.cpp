
#include "parse_configFile.hpp"
#include "Server.hpp"

// #define TEST_CONFIG_FILE

#ifdef TEST_CONFIG_FILE
int main() 
{

    std::ifstream file("conf/default.toml");

    if (!file)
    {
        std::cerr << "Failed to open the file." << std::endl;
        return 1;
    }

    // Read the file content into a buffer string
    std::string line;
    std::ostringstream bufferStream;
    while (std::getline(file, line))
        bufferStream << line << '\n';

    std::string buffer = bufferStream.str();

    std::vector<ServerSettings> servers;
    splitByServer(buffer, servers);
    parseDirectives(servers);

    for (size_t i = 0; i < servers.size(); ++i)
    {
        std::cout << "port         : " << servers[i].port << std::endl;
        std::cout << "host         : " << inet_ntoa(servers[i].host) << std::endl;
        std::cout << "server       : " << servers[i].server_name << std::endl;
        std::cout << "root         : " << servers[i].root << std::endl;
        std::cout << "AUTOINDEX    : " << servers[i].autoindex << std::endl;
        std::cout << "upload_path  : " << servers[i].upload_path << std::endl;

        for (size_t j = 0; j < servers[i].index_vec.size(); ++j)
            std::cout << "indexs[" << j << "] : " << servers[i].index_vec[j] << std::endl;

        std::cout << "bodySize     : " << servers[i].client_max_body_size << std::endl;

        for (size_t j = 0; j < servers[i].vec_of_locations.size(); ++j)
        {
            std::cout << "\n  ==LOCATION=== \n";
            std::map<std::string, std::string>::const_iterator map_iter;
            for (map_iter = servers[i].vec_of_locations[j].begin(); map_iter != servers[i].vec_of_locations[j].end(); ++map_iter)
            {
                std::cout << map_iter->first << " : " << map_iter->second << std::endl;
            }
        }

        std::cout << "\n  ==ERROR PAGE=== \n";
        std::map<int, std::string>::const_iterator map_iter;
        for (map_iter = servers[i].map_of_error_page.begin(); map_iter != servers[i].map_of_error_page.end(); ++map_iter)
        {
            std::cout << map_iter->first << " : " << map_iter->second << std::endl;
        }

        std::cout << "=============================================\n";
    }

   

    
    return 0;
}
#endif

bool isNumber(const std::string& input) {
    std::istringstream iss(input);
    double number;
    return (iss >> number) && (iss.eof());
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cerr << "Usage: ./webserv <config_file>" << std::endl;
        return 1;
    }

    std::string port = argv[1];
    if (!isNumber(port)) {
        std::cerr << "Invalid port number" << std::endl;
        return 1;
    }
    Server server = Server();
    server.run();
}





