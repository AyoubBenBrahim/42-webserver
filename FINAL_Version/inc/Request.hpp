#pragma once

# include <map>
# include <vector>
# include <string>
# include <sstream>
# include <iostream>
# include <fstream>
#include <macros.hpp>
#include <unistd.h>

#include <Utils.hpp>
#include <Parsing.hpp>


class Request {
	protected :
		std::string							bodyFile;
		bool								headEnd;
		std::string							headstr;
		std::map<std::string, std::string>	head;
		std::ofstream						outputFile;
		size_t								total;
		std::string							uploadHead;
		std::string							rest;
		std::string							body;

	public :
		~Request();
		Request();
	
		void									setHead( std::map<std::string, std::string>  head);
		std::map<std::string, std::string>&		getHead();
		void									addToHead(std::string key, std::string value);
		std::string&							getValueByKey(std::string key);
		size_t									getContentLength();
		bool									getHeadEnd() const;
		const	std::string						&getHeadStr() const;
		void									appendHead(char* buffer, int len);
		void									appendFile(char* buffer, int len);
        void									clear();
		const	std::string						&getUploadHead() const;
		void									setUploadHead(std::string uploadHead);
		const	std::string						&getRest() const;
		void									setRest(std::string rest);
		void									setBody(std::string body);
		// const	std::string						&getBodyFile() const;
		void									closeOutFile();
		const	size_t							&getTotal() const;
		std::string& 							getBody();
};