#include "ParseConfigFile.hpp"

void ParseConfigFiletrim(std::string &str)
{
    std::string whitespace = " \t\r\n";
    size_t start = str.find_first_not_of(whitespace);
    size_t end = str.find_last_not_of(whitespace);

    if (start != std::string::npos && end != std::string::npos)
        str = str.substr(start, end - start + 1);
    else
        str.clear();
}

int countSubstr(const std::string &str, const std::string &substr)
{
    int count = 0;
    std::string::size_type pos = 0;
    while ((pos = str.find(substr, pos)) != std::string::npos)
    {
        ++count;
        pos += substr.length();
    }
    return count;
}

bool is_comment(std::string line)
{
    size_t pos = line.find_first_not_of(" ");

    if (pos != std::string::npos && line[pos] == '#')
        return true;
    return false;
}

int strToInt(const std::string &str)
{
    char *endPtr;
    long value = strtol(str.c_str(), &endPtr, 10);
    if (*endPtr != '\0' || endPtr == str.c_str())
        return -1;

    if (value < std::numeric_limits<int>::min() || value > std::numeric_limits<int>::max())
        return -1;

    return static_cast<int>(value);
}

bool isValidPath(const std::string &path)
{
    // DIR *dir = opendir(path.c_str());
    // if (dir != NULL)
    // {
    //     closedir(dir);
    //     return true;
    // }
    // return false;

    std::ifstream file(path.c_str());
    return file.good();
}

bool isInRange(int num, int lower_bound, int upper_bound)
{
    return (num >= lower_bound && num <= upper_bound);
}

bool indentation(std::string line, size_t expected_spaces)
{
    size_t num_spaces = 0;

    num_spaces = line.find_first_not_of(" ");

    if (num_spaces == std::string::npos)
    {
        num_spaces = line.length();
        return false;
    }

    return (num_spaces == expected_spaces);
}

in_addr parseIPAddress(const char *ipAddressString, int serverNbr)
{
    in_addr address;
    DirectiveType directiveType = HOST;

    // Use inet_addr to convert the string to an IP address
    if (inet_addr(ipAddressString) != INADDR_NONE)
        address.s_addr = inet_addr(ipAddressString);
    else
        manageDirectivesErrors(serverNbr, directiveType);

    return address;
}

void manageDirectivesErrors(int serverNumber, DirectiveType directiveType)
{
    switch (directiveType)
    {
    case PORT:
        std::cerr << "[ERROR] in Server[" << serverNumber + 1 << "].PORT" << std::endl;
        std::cerr << "  Invalid Port number for server " << std::endl;
        break;
    case HOST:
        std::cerr << "[ERROR] in Server[" << serverNumber + 1 << "].HOST" << std::endl;
        std::cout << "  The IP Address is not valid." << std::endl;
        break;
    case SERVER_NAME:
        std::cerr << "[ERROR] in Server[" << serverNumber + 1 << "].SERVER_NAME" << std::endl;
        std::cerr << "  a valid server_name syntax \"webserv.com\"" << std::endl;
        break;
    case ROOT:
        std::cerr << "[ERROR] Invalid root directory in Server[" << serverNumber + 1 << "].ROOT" << std::endl;
        break;
    case INDEX:
        std::cerr << "[ERROR] Invalid index file in Server[" << serverNumber + 1 << "].INDEX" << std::endl;
        break;
    case CLIENT_MAX_BODY_SIZE:
        std::cerr << "[ERROR] in Server[" << serverNumber + 1 << "].CLIENT_MAX_BODY_SIZE" << std::endl;
        std::cerr << "  Invalid max body size." << std::endl;
        std::cerr << "  valid format: 10B 10K 10M 10G" << std::endl;
        break;
    case DIRECTIVE_AUTOINDEX:
        std::cerr << "[ERROR] in Server[" << serverNumber + 1 << "].AUTOINDEX" << std::endl;
        std::cerr << "  Invalid autoIndex value." << std::endl;
        std::cerr << "  supported values : ON OFF" << std::endl;
        break;
    case DIRECTIVE_UNKNOWN:
        std::cerr << "[ERROR] Unknown directive in Server[" << serverNumber + 1 << "]" << std::endl;
        break;
    case UPLOAD:
        std::cerr << "[ERROR] Invalid download path in Server[" << serverNumber + 1 << "]" << std::endl;
        break;
    default:
        std::cerr << "[ERROR] Unknown error occurred for server " << serverNumber + 1 << std::endl;
        break;
    }

    exit(0);
}

void manageContextsErrors(int serverNumber, ContextType contextType, int contextID)
{
    switch (contextType)
    {
    case URI:
        std::cerr << "[ERROR] in Server[" << serverNumber + 1 << "].Location[" << contextID << "].URI" << std::endl;
        std::cerr << "  Invalid URI for server " << std::endl;
        break;
    case ALLOW:
        std::cerr << "[ERROR] in Server[" << serverNumber + 1 << "].Location[" << contextID << "].ALLOW_METHODS" << std::endl;
        std::cout << "  Not Allowed Methods For Server." << std::endl;
        break;
    case CGI_PATH:
        std::cerr << "[ERROR] in Server[" << serverNumber + 1 << "].Location[" << contextID << "].CGI_PATH" << std::endl;
        std::cerr << "   Invalid Path/file." << std::endl;
        break;
    case CONTEXT_AUTOINDEX:
        std::cerr << "[ERROR] Invalid root directory in Server[" << serverNumber + 1 << "].Location[" << contextID << "].AUTOINDEX" << std::endl;
        std::cerr << "  Invalid autoIndex value." << std::endl;
        std::cerr << "  supported values : ON OFF" << std::endl;
        break;
    case RETURN:
        std::cerr << "[ERROR] Return syntax in Server[" << serverNumber + 1 << "].Location[" << contextID << "].RETURN" << std::endl;
        break;

    case ERROR:
        std::cerr << "[ERROR] Invalid Error Code in Server[" << serverNumber + 1 << "].error_page , number : " << contextID << "" << std::endl;
        break;

    case CONTEXT_UNKNOWN:
        std::cerr << "[ERROR] Unknown CONTEXT in Server[" << serverNumber + 1 << "].Location[" << contextID << "]" << std::endl;
        break;

    default:
        std::cerr << "[ERROR] Unknown error occurred for server " << serverNumber + 1 << std::endl;
        break;
    }

    exit(0);
}

std::size_t get_Body_Size_In_Bytes(const std::string &body_size, int serverNbr)
{
    std::size_t bytes = 0;
    std::string unit;
    std::string temp;
    double value = 0.0;
    DirectiveType directiveType = CLIENT_MAX_BODY_SIZE;

    if (body_size.front() != '"' || body_size.back() != '"')
        manageDirectivesErrors(serverNbr, directiveType);

    int quote_count = std::count(body_size.begin(), body_size.end(), '\"');

    if (quote_count != 2)
        manageDirectivesErrors(serverNbr, directiveType);

    temp = body_size.substr(1, body_size.size() - 2);

    trim(temp);

    if (temp.empty())
        temp = "0B";

    for (size_t i = 0; i < temp.size() - 1; i++)
        if (!std::isdigit(temp[i]))
            manageDirectivesErrors(serverNbr, directiveType);

    std::size_t unitPos = temp.find_first_of("KGBM");
    if (unitPos != std::string::npos)
    {
        value = std::atof(temp.substr(0, unitPos).c_str());
        unit = temp.substr(unitPos);
    }
    else
        manageDirectivesErrors(serverNbr, directiveType);

    if (unit == "B")
        bytes = static_cast<std::size_t>(value);
    else if (unit == "K")
        bytes = static_cast<std::size_t>(value * 1024);
    else if (unit == "M")
        bytes = static_cast<std::size_t>(value * 1024 * 1024);
    else if (unit == "G")
        bytes = static_cast<std::size_t>(value * 1024 * 1024 * 1024);
    else
        manageDirectivesErrors(serverNbr, directiveType);

    return bytes;
}

std::vector<std::string> getIndexes(std::string value, int serverNbr)
{
    std::vector<std::string> index_vec;
    DirectiveType directiveType = INDEX;

    if (value.front() != '[' || value.back() != ']' || value[value.size() - 2] == ',')
        manageDirectivesErrors(serverNbr, directiveType);

    std::stringstream ss(value.substr(1, value.size() - 2));

    std::string elem;

    while (std::getline(ss, elem, ','))
    {
        trim(elem);
        if (elem.front() != '"' || elem.back() != '"')
            manageDirectivesErrors(serverNbr, directiveType);

        elem = elem.substr(1, elem.size() - 2);
        trim(elem);

        int quote_count = std::count(elem.begin(), elem.end(), '\"');
        int dot_count = std::count(elem.begin(), elem.end(), '.');

        if (quote_count != 0 || dot_count != 1)
            manageDirectivesErrors(serverNbr, directiveType);

        if (elem.empty())
            manageDirectivesErrors(serverNbr, directiveType);

        index_vec.push_back(elem);
    }

    return index_vec;
}

std::string getAllowedMethods(std::string value, int serverNbr, int locationID)
{
    std::vector<std::string> allowed_methods;
    ContextType contextType = ALLOW;

    if (value.front() != '[' || value.back() != ']' || value[value.size() - 2] == ',')
        manageContextsErrors(serverNbr, contextType, locationID);

    std::stringstream ss(value.substr(1, value.size() - 2));

    std::string elem;

    while (std::getline(ss, elem, ','))
    {
        trim(elem);
        if (elem.front() != '"' || elem.back() != '"')
            manageContextsErrors(serverNbr, contextType, locationID);

        elem = elem.substr(1, elem.size() - 2);
        trim(elem);

        int quote_count = std::count(elem.begin(), elem.end(), '\"');

        if (quote_count != 0)
            manageContextsErrors(serverNbr, contextType, locationID);

        if (elem.empty())
            manageContextsErrors(serverNbr, contextType, locationID);

        if (elem != "GET" && elem != "POST" && elem != "DELETE")
            manageContextsErrors(serverNbr, contextType, locationID);

        allowed_methods.push_back(elem);
    }

    std::string methods;
    for (std::vector<std::string>::const_iterator it = allowed_methods.begin(); it != allowed_methods.end(); ++it)
    {
        methods += *it;
        if (it != allowed_methods.end() - 1)
            methods += ",";
    }

    return methods;
}

std::string extractServerName(std::string value, int serverNbr)
{
    DirectiveType directiveType = SERVER_NAME;

    trim(value);
    if (value.front() != '"' || value.back() != '"')
        manageDirectivesErrors(serverNbr, directiveType);

    value = value.substr(1, value.size() - 2);
    trim(value);

    int quote_count = std::count(value.begin(), value.end(), '\"');

    if (quote_count != 0)
        manageDirectivesErrors(serverNbr, directiveType);

    if (value.empty())
        manageDirectivesErrors(serverNbr, directiveType);

    return value;
}

int getErrorpageKey(std::string key, int serverNbr, int errorPageLineNbr)
{
    ContextType contextType = ERROR;

    trim(key);

    int _key = strToInt(key);
    if (_key == -1)
        manageContextsErrors(serverNbr, contextType, errorPageLineNbr);

    if (key[0] == '4')
    {
        if (!isInRange(_key, 400, 417))
            manageContextsErrors(serverNbr, contextType, errorPageLineNbr);
    }
    else if (key[0] == '5')
    {
        if (!isInRange(_key, 500, 505))
            manageContextsErrors(serverNbr, contextType, errorPageLineNbr);
    }
    else
        manageContextsErrors(serverNbr, contextType, errorPageLineNbr);

    return _key;
}

std::string getErrorpageValue(std::string key, std::string value, int serverNbr, int errorPage)
{
    ContextType contextType = ERROR;

    trim(value);

    if (value.front() != '"' || value.back() != '"')
        manageContextsErrors(serverNbr, contextType, errorPage);

    value = value.substr(1, value.size() - 2);
    trim(value);

    if (value.empty())
        manageContextsErrors(serverNbr, contextType, errorPage);

    if (key[0] == '4' && value != "/errors/4xx.html")
        manageContextsErrors(serverNbr, contextType, errorPage);

    else if (key[0] == '5' && value != "/errors/5xx.html")
        manageContextsErrors(serverNbr, contextType, errorPage);

    return value;
}

std::string getRoot(std::string value, int serverNbr)
{
    DirectiveType directiveType = ROOT;

    trim(value);
    if (value.front() != '"' || value.back() != '"')
        manageDirectivesErrors(serverNbr, directiveType);

    value = value.substr(1, value.size() - 2);
    trim(value);

    int quote_count = std::count(value.begin(), value.end(), '\"');

    if (quote_count != 0)
        manageDirectivesErrors(serverNbr, directiveType);

    if (value.empty())
        manageDirectivesErrors(serverNbr, directiveType);

    if (!isValidPath(value))
    {
        std::cerr << "[ERROR] in Server[" << serverNbr + 1 << "].Root" << std::endl;
        std::cerr << "   Invalid Path/Directory." << std::endl;
        exit(0);
    }

    return value;
}

std::string getContext(std::string value, int serverNbr, ContextType contextType, int locationID)
{
    trim(value);
    if (value.front() != '"' || value.back() != '"')
        manageContextsErrors(serverNbr, contextType, locationID);

    value = value.substr(1, value.size() - 2);
    trim(value);

    int quote_count = std::count(value.begin(), value.end(), '\"');

    if (quote_count != 0)
        manageContextsErrors(serverNbr, contextType, locationID);

    if (value.empty())
        manageContextsErrors(serverNbr, contextType, locationID);

    return value;
}

std::string getAuoIndexValue(std::string value, int serverNbr)
{
    DirectiveType directiveType = DIRECTIVE_AUTOINDEX;

    trim(value);
    if (value.front() != '"' || value.back() != '"')
        manageDirectivesErrors(serverNbr, directiveType);

    value = value.substr(1, value.size() - 2);
    trim(value);

    int quote_count = std::count(value.begin(), value.end(), '\"');

    if (quote_count != 0)
        manageDirectivesErrors(serverNbr, directiveType);

    if (value.empty())
        manageDirectivesErrors(serverNbr, directiveType);

    if (value != "ON" && value != "OFF")
        manageDirectivesErrors(serverNbr, directiveType);

    return value;
}

std::string getUploadPath(std::string value, int serverNbr)
{
    DirectiveType directiveType = UPLOAD;

    trim(value);
    if (value.front() != '"' || value.back() != '"')
        manageDirectivesErrors(serverNbr, directiveType);

    value = value.substr(1, value.size() - 2);
    trim(value);

    int quote_count = std::count(value.begin(), value.end(), '\"');

    if (quote_count != 0)
        manageDirectivesErrors(serverNbr, directiveType);

    if (value.empty())
        manageDirectivesErrors(serverNbr, directiveType);

    if (!isValidPath(value))
        manageDirectivesErrors(serverNbr, directiveType);

    return value;
}

std::string getCGIpath(std::string value, int serverNbr, ContextType contextType, int locationID)
{
    trim(value);
    if (value.front() != '"' || value.back() != '"')
        manageContextsErrors(serverNbr, contextType, locationID);

    value = value.substr(1, value.size() - 2);
    trim(value);

    int quote_count = std::count(value.begin(), value.end(), '\"');

    if (quote_count != 0)
        manageContextsErrors(serverNbr, contextType, locationID);

    if (value.empty())
        manageContextsErrors(serverNbr, contextType, locationID);

    if (!isValidPath(value))
        manageContextsErrors(serverNbr, contextType, locationID);

    return value;
}

void splitByServer(const std::string &buffer, std::vector<ServerSettings> &servers)
{
    std::istringstream iss(buffer);
    std::string line;
    std::ostringstream currentChunk;
    ServerSettings server;

    if (std::getline(iss, line))
    {
        if (line.empty())
        {
            std::cout << "[ERROR] First Line of the File is Empty." << std::endl;
            exit(0);
        }
        if (line != "[[server]]")
        {
            std::cout << "[ERROR] File should start with [[server]] Directive." << std::endl;
            exit(0);
        }
    }
    else
    {
        std::cout << "[ERROR] while Reading the File" << std::endl;
        exit(0);
    }

    iss.clear();
    iss.seekg(0, std::ios::beg);

    while (std::getline(iss, line))
    {
        if (is_comment(line))
            continue;

        if (line.find("[[server]]") != std::string::npos)
        {
            if (indentation(line, 0))
            {
                std::string chunk = currentChunk.str();

                if (!chunk.length())
                    continue;
                if (chunk.length() == 1)
                {
                    std::cerr << "[ERROR]: Empty Server Directive \"" << line << "\"" << std::endl;
                    exit(0);
                }

                server.serverChunk = chunk;
                currentChunk.str("");
                currentChunk.clear();

                servers.push_back(server);
                server = ServerSettings();

                continue;
            }
            else
            {
                std::cerr << "[ERROR]: invalid Directive Indentation \"" << line << "\"" << std::endl;
                exit(0);
            }
        }

        currentChunk << line << '\n';
    }
    std::string lastChunk = currentChunk.str();
    if (!lastChunk.empty())
    {
        server = ServerSettings();
        server.serverChunk = lastChunk;
        servers.push_back(server);
    }
}

void supportedDirectives(std::map<std::string, DirectiveType> &supportedDirectivesMap)
{
    supportedDirectivesMap["port"] = PORT;
    supportedDirectivesMap["host"] = HOST;
    supportedDirectivesMap["server_name"] = SERVER_NAME;
    supportedDirectivesMap["root"] = ROOT;
    supportedDirectivesMap["index"] = INDEX;
    supportedDirectivesMap["client_max_body_size"] = CLIENT_MAX_BODY_SIZE;
    supportedDirectivesMap["autoindex"] = DIRECTIVE_AUTOINDEX;
    supportedDirectivesMap["upload_path"] = UPLOAD;
}

void checkForMandatoryDirectives(const std::map<std::string, std::string> &encounteredDirectivesMap, int serverID)
{
    std::set<std::string> mandatoryDirectives;
    mandatoryDirectives.insert("port");
    mandatoryDirectives.insert("host");
    mandatoryDirectives.insert("root");

    for (std::set<std::string>::iterator it = mandatoryDirectives.begin(); it != mandatoryDirectives.end(); ++it)
    {
        const std::string &directive = *it;

        std::map<std::string, std::string>::const_iterator encounteredDirectiveIterator = encounteredDirectivesMap.find(directive);
        if (encounteredDirectiveIterator == encounteredDirectivesMap.end())
        {
            std::cerr << "[ERROR] in Server[" << serverID + 1 << "]" << std::endl;
            std::cerr << "   Directive \"" << directive << "\" is Mandatory." << std::endl;
            exit(0);
        }
    }
}

void getValidDirectives(std::vector<ServerSettings> &server, std::map<std::string, std::string> encounteredDirectivesMap, int serverID)
{
    std::map<std::string, DirectiveType> supportedDirectivesMap;

    supportedDirectives(supportedDirectivesMap);

    checkForMandatoryDirectives(encounteredDirectivesMap, serverID);

    for (std::map<std::string, std::string>::iterator it = encounteredDirectivesMap.begin(); it != encounteredDirectivesMap.end(); ++it)
    {
        const std::string &key = it->first;
        std::string &value = it->second;

        std::map<std::string, DirectiveType>::iterator supportedDirectiveIterator = supportedDirectivesMap.find(key);
        if (supportedDirectiveIterator == supportedDirectivesMap.end())
        {
            DirectiveType directiveType = DIRECTIVE_UNKNOWN;
            manageDirectivesErrors(serverID, directiveType);
        }
        else
        {
            DirectiveType directiveType = supportedDirectiveIterator->second;
            switch (directiveType)
            {
            case PORT:
            {
                std::istringstream iss(value);
                if (!(iss >> server[serverID].port) || !isInRange(server[serverID].port, 0, 49151))
                    manageDirectivesErrors(serverID, directiveType);
            }
            break;
            case HOST:
                if (value.front() != '"' || value.back() != '"')
                    manageDirectivesErrors(serverID, directiveType);
                server[serverID].host = parseIPAddress((value.substr(1, value.size() - 2)).c_str(), serverID);
                break;
            case SERVER_NAME:
                server[serverID].server_name = extractServerName(value, serverID);
                break;
            case ROOT:
                server[serverID].root = getRoot(value, serverID);
                break;
            case INDEX:
                server[serverID].index_vec = getIndexes(value, serverID);
                break;
            case CLIENT_MAX_BODY_SIZE:
                server[serverID].client_max_body_size = get_Body_Size_In_Bytes(value, serverID);
                break;
            case DIRECTIVE_AUTOINDEX:
                server[serverID].autoindex = getAuoIndexValue(value, serverID);
                break;
            case UPLOAD:
                server[serverID].upload_path = getUploadPath(value, serverID);
                break;
            default:
                break;
            }
        }
    }
}

void supportedContexts(std::map<std::string, ContextType> &supportedContextsMap)
{
    supportedContextsMap["uri"] = URI;
    supportedContextsMap["allow_methods"] = ALLOW;
    supportedContextsMap["cgi_path"] = CGI_PATH;
    supportedContextsMap["autoindex"] = CONTEXT_AUTOINDEX;
    supportedContextsMap["return"] = RETURN;
}

void getValidContexts(std::vector<ServerSettings> &server, std::map<std::string, std::string> encounteredContextsMap, int serverID)
{
    std::map<std::string, ContextType> supportedContextsMap;

    supportedContexts(supportedContextsMap);

    std::map<std::string, std::string> map_of_locations;
    int locationID = server[serverID].vec_of_locations.size();
    locationID = (!locationID) ? 1 : locationID;

    for (std::map<std::string, std::string>::iterator it = encounteredContextsMap.begin(); it != encounteredContextsMap.end(); ++it)
    {
        const std::string &key = it->first;
        std::string &value = it->second;

        std::map<std::string, ContextType>::iterator supportedContextsIterator = supportedContextsMap.find(key);

        if (supportedContextsIterator == supportedContextsMap.end())
        {
            ContextType contextType = CONTEXT_UNKNOWN;
            manageContextsErrors(serverID, contextType, locationID);
        }
        else
        {
            ContextType contextType = supportedContextsIterator->second;
            switch (contextType)
            {
            case URI:
                map_of_locations[key] = getContext(value, serverID, contextType, locationID);
                break;
            case ALLOW:
                map_of_locations[key] = getAllowedMethods(value, serverID, locationID);
                break;
            case CGI_PATH:
                map_of_locations[key] = getCGIpath(value, serverID, contextType, locationID);
                break;
            case CONTEXT_AUTOINDEX:
                map_of_locations[key] = getAuoIndexValue(value, serverID);
                break;
            case RETURN:
                map_of_locations[key] = getContext(value, serverID, contextType, locationID);
                break;
            default:
                break;
            }
        }
    }
    server[serverID].vec_of_locations.push_back(map_of_locations);
}

void mannageContexts(std::vector<ServerSettings> &server, int serverID, ChildSectionStruct childSection)
{
    std::istringstream content(childSection.content);
    std::string inner_line;

    std::map<std::string, std::string> encounteredContextsMap;

    int lineNbr = 1;
    while (std::getline(content, inner_line))
    {
        size_t pos = inner_line.find("=");
        if (pos != std::string::npos && indentation(inner_line, 8))
        {
            std::string key = inner_line.substr(0, pos);
            std::string val = inner_line.substr(pos + 1);
            trim(key);
            trim(val);
            trim(childSection.header);

            if (childSection.header == "[[server.location]]")
                encounteredContextsMap[key] = val;
            else if (childSection.header == "[[server.error_page]]")
            {
                int _keyAsInt = getErrorpageKey(key, serverID, lineNbr);
                server[serverID].map_of_error_page[_keyAsInt] = getErrorpageValue(key, val, serverID, lineNbr);
                lineNbr++;
            }
            else
            {
                std::cerr << "Error: invalid Block Directive \"" << childSection.header << "\"" << std::endl;
                exit(0);
            }
        }
        if (pos == std::string::npos)
        {
            std::cerr << "[ERROR]: invalid Key/value pair \"" << inner_line << "\"" << std::endl;
            exit(0);
        }
    }

    if (!encounteredContextsMap.empty())
        getValidContexts(server, encounteredContextsMap, serverID);
}

void parseDirectives(std::vector<ServerSettings> &server)
{
    int emptyLinesCounter = 0;
    int directivesCounter = server.size();

    for (size_t i = 0; i < server.size(); ++i)
    {
        std::istringstream iss(server[i].serverChunk);
        std::string line;
        std::ostringstream currentChildContent;
        std::string currentChildHeader;

        std::map<std::string, std::string> encounteredDirectives;

        while (std::getline(iss, line))
        {
            if (line.empty())
            {
                emptyLinesCounter++;
                continue;
            }

            if (!indentation(line, 8) && !indentation(line, 4))
            {
                std::cout << " *** INDENTATION ERROR *** \"" << line << "\"" << std::endl;
                exit(0);
            }

            if (line.find("[[server.") != std::string::npos && line.find("[[server.") < line.find("]]"))
            {
                if (indentation(line, 4))
                {
                    std::string childHeader = line.substr(2, line.length());
                    std::string childContent = currentChildContent.str();

                    if (!currentChildHeader.empty() && !childContent.empty())
                    {
                        int dot_count = std::count(currentChildHeader.begin(), currentChildHeader.end(), '.');
                        if (dot_count != 1)
                        {
                            std::cerr << "Error: UNKNOWN Block Directive \"" << currentChildHeader << "\"" << std::endl;
                            exit(0);
                        }

                        ChildSectionStruct childSection;

                        childSection.header = currentChildHeader;
                        childSection.content = childContent;

                        mannageContexts(server, i, childSection);
                    }

                    currentChildHeader = childHeader;
                    currentChildContent.str("");
                    currentChildContent.clear();
                    continue;
                }
            }
            if (indentation(line, 4))
            {
                trim(line);
                size_t equalsPos = line.find('=');
                if (equalsPos != std::string::npos)
                {
                    std::string key = line.substr(0, equalsPos);
                    std::string value = line.substr(equalsPos + 1);

                    trim(key);
                    trim(value);

                    encounteredDirectives[key] = value;
                }
                else
                {
                    std::cerr << "[ERROR]: invalid Key/value pair \"" << line << "\"" << std::endl;
                    exit(0);
                }
            }
            currentChildContent << line << '\n';
        }

        // Handle the last child section
        std::string lastChildContent = currentChildContent.str();
        if (!currentChildHeader.empty() && !lastChildContent.empty())
        {
            std::map<std::string, std::string> map_of_locations;
            std::map<std::string, std::string> map_of_errors;
            ChildSectionStruct childSection;

            childSection.header = currentChildHeader;
            childSection.content = lastChildContent;

            mannageContexts(server, i, childSection);
        }
        getValidDirectives(server, encounteredDirectives, i);
        directivesCounter += countSubstr(server[i].serverChunk, "[[");
    }

    if (emptyLinesCounter != directivesCounter)
    {
        std::cerr << "[ERROR]: " << emptyLinesCounter - directivesCounter << " Extra Empty Lines in the file." << std::endl;
        exit(0);
    }
}

void parseConfigFile(const std::string &configFilePath, std::vector<ServerSettings> &http_servers)
{
     // std::ifstream file("conf/default.toml");
    std::ifstream file(configFilePath.c_str());

    if (!file)
        throw std::runtime_error("[ERROR]: Unable to open the file \"" + configFilePath + "\"");

    // Read the file content into a buffer string
    std::string line;
    std::ostringstream bufferStream;
    while (std::getline(file, line))
        bufferStream << line << '\n';

     // std::stringstream ss;
    // ss << file.rdbuf(); // check if this allowed in c++98
    // buffer = ss.str();

    std::string buffer = bufferStream.str();
    splitByServer(buffer, http_servers);
    parseDirectives(http_servers);
}