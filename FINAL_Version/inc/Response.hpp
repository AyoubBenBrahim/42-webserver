#pragma once

#include <Request.hpp>
#include <sys/wait.h>
#include <fcntl.h>
#include <macros.hpp>
#include <dirent.h>
#include <sys/stat.h>
#include <Cgi.hpp>

class Response {
	private:
		std::string					response;
		std::string					header;
		std::string					body;
		std::string 				mimeType;
		std::map<int, std::string>	statusCode;
		Request*					request;
		t_config					config;
		int							locationIndex;
		int							serverIndex;
		std::ifstream				currentFile;
		std::string					path;
		bool						headerDone;
		long long					contentLength;
		std::ofstream				upload_of;
		bool						uploadDone;

		std::string 				upload_head;
		size_t						fileSize;
		size_t						uploadPosition;
		bool						defaul_page_flag;
		bool						uploaded;
		Cgi							*cgi;
		bool						chunked;
	public:
		Response();
		Response(int status, t_config& config);

		Response(Request* request, t_config& config);
		Response& operator=(const Response& response);
		Response(const Response& response);

		~Response();

		// Getters;
		const std::string&	getResponse();
		const Request*	getRequest();
		const t_location&	getLocation();
		const t_server&		getServer();
		bool    			isItChunked();
		bool	 			getUploaded();
		//Setters
		void				setContentType(const std::string& extension);
		void				initStatusCodeMap();
		void				setHeader(std::string& _header);
		void				setHeader(int status);
		void				setBody(const std::string& body);
		void				setResponse();
		void				setResponse(std::string& resp);
		void    			setResToChunked(bool chunked);
	private:
		std::string			matching();
		void				serverExists();
		void				locationExists();
		const std::string&	returnError(int status);
		void				methodNotAllowed();
		void				locationRedirection();
		void				deleteMethod(DIR* dir);
		bool				deleteRequested();
		void				postMethod(DIR* dir, const std::string& path);



		void				listDirectory(DIR* dir);
		bool				redirectDir();
		void				executeCgi(const std::string& path, const std::string& cgiExecutable);
		void				servFile(std::string& filePath, int status, int errorStatus);
		std::string			fileToString(const std::string& fileName, int errorStatus);
		void				defaultPage();
		bool				uploadFile(std::string& uploadPath);
		std::string			getBoundary();
		void				checkCgiHeader();
		bool				uploadFile(std::ifstream& in);


	public:
		// void				closeFile();
		std::string			buildResponse( void );

};
