#pragma once

#include <map>
#include <string>
#include <sstream>
#include <libc.h>
#include <cmath>
#include "returnCodes.h"

class Response {
 public:
	typedef std::map<std::string, std::string> Headers;
	explicit Response(int code = 500);
	std::string toString() const;
	void SetHeader(const std::string &title, const std::vector<std::string> &multi_content);
	void SetHeader(const std::string &title, const std::string &content);
	void SetHeader(const std::string &title, size_t content);
	void SetCode(int code);
	std::string GetHeader(const std::string &title) const;
	void SetBody(const std::string &body);
	void SetDefaultContent(const std::map<int, std::string> &errorPages);
	int GetCode() const;

 private:
	static std::string GetCurDate();
	void generate_error_page(int code);
	void get_error_page_from_file(const std::string &file);
	int code_;
	Headers headers_;
	std::string body_;
};

class RespException : public std::exception {
 public:
	virtual ~RespException() throw();
	const char *what() const throw();
	explicit RespException(const Response &rsp) throw();
	const Response &GetRsp() const;
 private:
	Response rsp_;
};
