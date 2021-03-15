#pragma once

#include <map>
#include <string>
#include <sstream>
#include <libc.h>
#include "returnCodes.h"


class Response {
public:
	typedef std::map<std::string, std::string> Headers;
	explicit Response(int code);
	std::string Generate();
	void SetHeader(const std::string &title, const std::string &content);
  	void SetHeader(const std::string &title, const std::vector<std::string>& multi_content);

	void SetBody(const std::string& body);
	std::string GetHeader(const std::string &title) const;

private:
	std::string GetCurDate();

	void generate_error_page(int code);
	int code_;
	Headers headers_;
	std::string body_;
};