#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <set>
#include <string>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <unistd.h>

struct ChildSectionStruct
{
    int serverParent;
    std::string header;
    std::string content;
};

enum DirectiveType
{
    PORT,
    HOST,
    SERVER_NAME,
    ROOT,
    INDEX,
    CLIENT_MAX_BODY_SIZE,
    DIRECTIVE_AUTOINDEX,
    UPLOAD,
    DIRECTIVE_UNKNOWN
};

enum ContextType
{
    URI,
    ALLOW,
    CGI_PATH,
    CONTEXT_AUTOINDEX,
    RETURN,
    ERROR,
    CONTEXT_UNKNOWN
};

class ServerSettings
{
public:
    int port;
    in_addr host;
    std::string server_name;
    std::string root;
    std::string autoindex;
    std::string upload_path;
    std::vector<std::string> index_vec;
    std::size_t client_max_body_size;
    std::map<int, std::string> map_of_error_page;
    std::vector<std::map<std::string, std::string> > vec_of_locations;
    std::string serverChunk;
};

void                        parseConfigFile(const std::string &configFilePath, std::vector<ServerSettings> &http_servers);
void                        splitByServer(const std::string &buffer, std::vector<ServerSettings> &servers);
int                         countSubstr(const std::string &str, const std::string &substr);
void                        trim(std::string &str);
bool                        indentation(std::string line, size_t expected_spaces);
int                         strToInt(const std::string &str);
bool                        isInRange(int num, int lower_bound, int upper_bound);
bool                        is_comment(std::string line);
bool                        isValidPath(const std::string& path);
void                        parseDirectives(std::vector<ServerSettings> &server);
void                        check_inter_block_space(std::vector<ServerSettings> &servers);
in_addr                     parseIPAddress(const char *ipAddressString, int serverNbr);
void                        manageDirectivesErrors(int serverNumber, DirectiveType directiveType);
void                        manageContextsErrors(int serverNumber, ContextType contextType, int contextID);
std::vector<std::string>    getIndexes(std::string value, int serverNbr);
std::size_t                 get_Body_Size_In_Bytes(const std::string &body_size, int serverNbr);
std::string                 getRoot(std::string value, int serverNbr);
std::string                 getContext(std::string value, int serverNbr, ContextType contextType, int locationID);
std::string                 getCGIpath(std::string value, int serverNbr, ContextType contextType, int locationID);
std::string                 getAuoIndexValue(std::string value, int serverNbr);
std::string                 getUploadPath(std::string value, int serverNbr);
void                        supportedDirectives(std::map<std::string, DirectiveType> &supportedDirectivesMap);
std::string                 extractServerName(std::string value, int serverNbr);
int                         getErrorpageKey(std::string key, int serverNbr, int errorPageLineNbr);
std::string                 getErrorpageValue(std::string key, std::string value, int serverNbr, int errorPage);
std::string                 getAllowedMethods(std::string value, int serverNbr, int locationID);
void                        supportedContexts(std::map<std::string, ContextType> &supportedContextsMap);
void                        getValidDirectives(std::vector<ServerSettings> &server, std::map<std::string, std::string> encounteredDirectivesMap, int serverID);
void                        getValidContexts(std::vector<ServerSettings> &server, std::map<std::string, std::string> encounteredContextsMap, int serverID);
void                        mannageContexts(std::vector<ServerSettings> &server, int serverID, ChildSectionStruct childSection);
void                        checkForMandatoryDirectives(const std::map<std::string, std::string>& encounteredDirectivesMap, int serverID);

