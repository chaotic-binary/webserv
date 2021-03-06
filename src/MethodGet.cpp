//
// Created by Mahmud Jego on 3/4/21.
//

#include "includes.h"
#include "Response.hpp"
#include "Request.h"
#include "mimeTypes.h"

std::string getMimeType(const std::string& file)
{
	static std::map<std::string, std::string> mimeTypes;
	if (mimeTypes.empty())
		initMimeType(mimeTypes);
	std::string exp =  &file.c_str()[file.rfind('.')];
	std::string res;
	if (mimeTypes.count(exp))
	{
		res = mimeTypes[exp];
	}
	else
		res = "text/plain";
	return res;
}

void tmpFunctionForResponse(int fd, const Request &req, const ServConfig& config)
{
	Response response1(fd, req, config); // TODO: mv all response <<

	std::string mimeType;
	std::string pwd = getcwd(0x0, 0);
	if (req.getReqTarget() == "/")
	{
		pwd +=  + "/www/" + config.getLocations()[0].getIndex();
		mimeType = getMimeType(config.getLocations()[0].getIndex());
	}
	else
	{
		pwd += "/www" + req.getReqTarget();
		mimeType = getMimeType(req.getReqTarget());
	}
	std::ifstream file(pwd);

	if (!file)
		exit(13);

	std::stringstream response_body;
	response_body << file.rdbuf();

	// Формируем весь ответ вместе с заголовками
	std::stringstream response;
	response << "HTTP/1.1 200 OK\r\n"
			 << "Version: HTTP/1.1\r\n"
			 << "Content-Type: " << mimeType << "; charset=utf-8\r\n"
			 << "Content-Length: " << response_body.str().length()
			 << "\r\n\r\n"
			 << response_body.str();

	int ret = send(fd, response.str().c_str(), response.str().length(), 0);
	if (ret == -1)
		exit(13);
}

