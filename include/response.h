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
	std::string Generate();
	void SetHeader(const std::string &title, const std::string &content);
	void SetHeader(const std::string &title, const std::vector<std::string> &multi_content);
	void SetBody(const std::string &body);
	std::string GetHeader(const std::string &title) const;
private:
	std::string GetCurDate() const;

	void generate_error_page(int code);
	int code_;
 public:
  int GetCode() const;
 private:
  Headers headers_;
	std::string body_;
};

class RespException : public std::exception
{
 public:
  	virtual ~RespException()  throw();
	const char * what() const throw();
  explicit RespException(const Response &rsp) throw();
  const Response &GetRsp() const;
 private:
  Response rsp_;
};
