#pragma once

# define REQ_METHOD			"Request-Method"
# define REQ_PATH			"REQUEST_URI"
# define REQ_HTTP_VERSION	"Server-Protocol"
# define REQ_SERVER_NAME	"Server-Name"
# define REQ_PORT			"Server-Port"
# define REQ_QUERY_STRING	"QUERY_STRING"
# define REQ_ENTITY_POST	"Entity_Post"
# define REQ_CONTENT_TYPE	"Content-Type"
# define REQ_TRANSFER		"Transfer-Encoding"
# define REQ_CONTENT_LENGTH "Content-Length"
# define REQ_CONNECTION 	"Connection"
# define REQ_COOKIES		"Cookie"

# define BUFFER_SIZE        1024
// Server
# define MAX_LEN			300000
// # define PORT				8080
# define BACKLOG			5
# define LAST_SEEN_TIMEOUT	6000000000


# define DEFAULT_400_STATUS_PAGE	"<!DOCTYPE html>\n<head>\n<title>400 Bad Request</title>\n<style>\nbody {\ntext-align: center;\npadding: 50px;\n}\nh1 {\nfont-size: 3em;\ncolor: #555;\n}\np {\nfont-size: 1.5em;\ncolor: #777;\n}\n</style>\n</head>\n<body>\n<h1>400 - Bad Request</h1>\n<p>Sorry, the server could not understand the request due to invalid syntax or other client-side errors.</p>\n</body>\n</html>"
# define DEFAULT_415_STATUS_PAGE	"<!DOCTYPE html>\n<head>\n<title>415 Unsupported Media Type</title>\n<style>\nbody {\ntext-align: center;\npadding: 50px;\n}\nh1 {\nfont-size: 3em;\ncolor: #555;\n}\np {\nfont-size: 1.5em;\ncolor: #777;\n}\n</style>\n</head>\n<body>\n<h1>415 - Unsupported Media Type</h1>\n<p>Sorry, the server cannot process the media type requested.</p>\n</body>\n</html>"
# define DEFAULT_201_STATUS_PAGE	"<!DOCTYPE html>\n<head>\n<title>201 Created</title>\n<style>\nbody {\ntext-align: center;\npadding: 50px;\n}\nh1 {\nfont-size: 3em;\ncolor: #555;\n}\np {\nfont-size: 1.5em;\ncolor: #777;\n}\n</style>\n</head>\n<body>\n<h1>201 - Created</h1>\n<p>The request has been fulfilled and resulted in a new resource being created.</p>\n</body>\n</html>"
# define DEFAULT_301_STATUS_PAGE	"<!DOCTYPE html>\n<head>\n<title>301 Moved Permanently</title>\n<style>\nbody {\ntext-align: center;\npadding: 50px;\n}\nh1 {\nfont-size: 3em;\ncolor: #555;\n}\np {\nfont-size: 1.5em;\ncolor: #777;\n}\n</style>\n</head>\n<body>\n<h1>301 - Moved Permanently</h1>\n<p>The requested resource has been permanently moved to a new location.</p>\n</body>\n</html>"
# define DEFAULT_403_STATUS_PAGE	"<!DOCTYPE html>\n<head>\n<title>403 Forbidden</title>\n<style>\nbody {\ntext-align: center;\npadding: 50px;\n}\nh1 {\nfont-size: 3em;\ncolor: #555;\n}\np {\nfont-size: 1.5em;\ncolor: #777;\n}\n</style>\n</head>\n<body>\n<h1>403 - Forbidden</h1>\n<p>Sorry, you do not have permission to access this resource.</p>\n</body>\n</html>"
# define DEFAULT_404_STATUS_PAGE	"<!DOCTYPE html>\n<head>\n<title>404 Not Found</title>\n<style>\nbody {\ntext-align: center;\npadding: 50px;\n}\nh1 {\nfont-size: 3em;\ncolor: #555;\n}\np {\nfont-size: 1.5em;\ncolor: #777;\n}\n</style>\n</head>\n<body>\n<h1>404 - Not Found</h1>\n<p>Sorry, the page you are looking for does not exist.</p>\n</body>\n</html>"
# define DEFAULT_405_STATUS_PAGE	"<!DOCTYPE html>\n<head>\n<title>405 Method Not Allowed</title>\n<style>\nbody {\ntext-align: center;\npadding: 50px;\n}\nh1 {\nfont-size: 3em;\ncolor: #555;\n}\np {\nfont-size: 1.5em;\ncolor: #777;\n}\n</style>\n</head>\n<body>\n<h1>405 - Method Not Allowed</h1>\n<p>The method specified in the request is not allowed for the resource identified.</p>\n</body>\n</html>"
# define DEFAULT_501_STATUS_PAGE	"<!DOCTYPE html>\n<head>\n<title>501 Not Implemented</title>\n<style>\nbody {\ntext-align: center;\npadding: 50px;\n}\nh1 {\nfont-size: 3em;\ncolor: #555;\n}\np {\nfont-size: 1.5em;\ncolor: #777;\n}\n</style>\n</head>\n<body>\n<h1>501 - Not Implemented</h1>\n<p>The server does not support the functionality required to fulfill the request.</p>\n</body>\n</html>"
# define DEFAULT_409_STATUS_PAGE	"<!DOCTYPE html>\n<head>\n<title>409 Conflict</title>\n<style>\nbody {\ntext-align: center;\npadding: 50px;\n}\nh1 {\nfont-size: 3em;\ncolor: #555;\n}\np {\nfont-size: 1.5em;\ncolor: #777;\n}\n</style>\n</head>\n<body>\n<h1>409 - Conflict</h1>\n<p>The request could not be completed due to a conflict with the current state of the target resource.</p>\n</body>\n</html>"
# define DEFAULT_500_STATUS_PAGE	"<!DOCTYPE html>\n<head>\n<title>500 Internal Server Error</title>\n<style>\nbody {\ntext-align: center;\npadding: 50px;\n}\nh1 {\nfont-size: 3em;\ncolor: #555;\n}\np {\nfont-size: 1.5em;\ncolor: #777;\n}\n</style>\n</head>\n<body>\n<h1>500 - Internal Server Error</h1>\n<p>Sorry, the server encountered an unexpected condition that prevented it from fulfilling the request.</p>\n</body>\n</html>"
# define DEFAULT_413_STATUS_PAGE	"<!DOCTYPE html>\n<head>\n<title>413 Request Entity Too Large</title>\n<style>\nbody {\ntext-align: center;\npadding: 50px;\n}\nh1 {\nfont-size: 3em;\ncolor: #555;\n}\np {\nfont-size: 1.5em;\ncolor: #777;\n}\n</style>\n</head>\n<body>\n<h1>413 - Request Entity Too Large</h1>\n<p>The server is refusing to process a request because the request entity is larger than the server is willing or able to process.</p>\n</body>\n</html>"
# define DEFAULT_504_STATUS_PAGE	"<!DOCTYPE html>\n<head>\n<title>504 Gateway Timeout</title>\n<style>\nbody {\ntext-align: center;\npadding: 50px;\n}\nh1 {\nfont-size: 3em;\ncolor: #555;\n}\np {\nfont-size: 1.5em;\ncolor: #777;\n}\n</style>\n</head>\n<body>\n<h1>504 - Gateway Timeout</h1>\n<p>timeout.</p>\n</body>\n</html>"
# define DEFAULT_505_STATUS_PAGE	"<!DOCTYPE html>\n<head>\n<title>505 HTTP Version Not Supported</title>\n<style>\nbody {\ntext-align: center;\npadding: 50px;\n}\nh1 {\nfont-size: 3em;\ncolor: #555;\n}\np {\nfont-size: 1.5em;\ncolor: #777;\n}\n</style>\n</head>\n<body>\n<h1>504 - Gateway Timeout</h1>\n<p>this http version is not supported</p>\n</body>\n</html>"

# define NOT_READDED                    400
# define BAD_REQUEST_STATUS				400
# define CREATED_STATUS					201
# define NOT_FOUND_STATUS				404
# define METHOD_NOT_ALLOWED_STATUS		405
# define FORBIDDEN_STATUS				403
# define NOT_IMPLEMENTED_STATUS			501
# define MOVED_PERMANENTLY_STATUS		301
# define NOT_MODIFIED_STATUS			304
# define CONFLICT_STATUS				409
# define NO_CONTENT_STATUS				204
# define INTERNAL_SERVER_ERROR_STATUS	500
# define INSUFFICIENT_STORAGE           500
# define CONTENT_TOO_LARGE_STATUS       413
# define UNSUPPORTED_MEDIA_TYPE_STATUS	415
# define GATEWAY_TIMEOUT_STATUS         504
# define GOOD_STATUS			        0
# define HTTP_VERSION_NOT_SUP_STATUS 	505



// PARSINGTYPE
# define BOUNDARY			1
# define CHUNKED			2
# define BOUNDARYCHUNKED	3


// parsing
/* ****** COLORS ****** */
# define RESET_COLOR	"\033[0m"
# define RED			"\033[1;31m"
# define GREEN			"\033[1;32m"
# define YELLOW			"\033[1;33m"
# define ORANGE			"\033[1;34m"
# define MAGENTA		"\033[1;35m"
# define CYAN			"\033[1;36m"
# define WHITE			"\033[1;37m"

# define ALLOWED_URI_CHARS      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%"
# define DEFAULT_CONFIG_FILE    "./conf/def.conf"

# define NO_LOC_PATH    		RED "Error: " GREEN << "location has no path." << RESET_COLOR << "\n"
# define INVALID_ARGUMENT		RED "Error: " GREEN << key << " Invalid argument." << RESET_COLOR << "\n"
# define NO_VALUE				RED "Error: " GREEN << key << " Directive has no value." << RESET_COLOR << "\n"
# define PRINT_LINE_AND_FILE	YELLOW "[" << __FILE__ << ":" << __LINE__ << "]\n" RESET_COLOR
# define INVALID_METHOD			RED "Error: " GREEN "Invalid method." << RESET_COLOR << "\n"
# define INVALID_LOC_DIRECTIVE	RED "Error: " GREEN "Invalid location Directive." << RESET_COLOR << "\n"
# define EXPECTED_SEM			RED "Error: " GREEN "expected ';' at end of declaration." << RESET_COLOR << "\n"
# define INVALID_DIRECTIVE		RED "Error: " GREEN "Invalid Directive." << RESET_COLOR << "\n"

# define DEFAULT_PORT			8080

# define DEFAULT_PAGE			"<!DOCTYPE html><html><head><title>Welcome to Webserv!</title><style>body{font-family:Arial,sans-serif;background-color:#f4f4f4;color:#333;margin:20px;}h1{color:#2e8b57;}p{color:#555;}ul{list-style-type:none;padding:0;}li{margin-bottom:10px;}a{color:#0066cc;text-decoration:none;font-weight:bold;}a:hover{text-decoration:underline;}</style></head><body><h1>Welcome to Webserv!</h1><p>If you see this page, the Webserv web server is successfully installed and working.</p><h2>GitHub Accounts:</h2><ul><li><a href=\"https://github.com/AyoubBenBrahim\">aybouras</a></li><li><a href=\"https://github.com/Ayoub-Mentag\">amentag</a></li><li><a href=\"https://github.com/mtigunit\">mtigunit</a></li></ul></body></html>"
# define UPLOADED_DEFAULT_PAGE	"<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Upload Done</title><style>body {font-family: \'Segoe UI\', Tahoma, Geneva, Verdana, sans-serif;background-color: #f8f9fa;color: #495057;text-align: center;margin: 50px;}#upload-container {background-color: #ffffff;border-radius: 8px;box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);padding: 20px;max-width: 400px;margin: 0 auto;}#upload-message {font-size: 24px;font-weight: bold;color: #28a745;margin-bottom: 20px;}#upload-message::after{content: \'\\2713\';font-size: 36px;display: block;color: #28a745;margin-top: 10px;}</style></head><body><div id=\"upload-container\"><div id=\"upload-message\">Upload Successful!</div></div></body></html>"
