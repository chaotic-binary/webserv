#pragma once

#include "ServConfig.h"
#include "Request.h"
#include <unistd.h>

#define NOT_FOUND "404"
#define FORBIDDEN "403"
#define OK "200"

class king_Response {
public:
	king_Response(int fd, const Request &req, const ServConfig& config);

	void generateHeaders();
	void generateBody();

	std::string getRespone();
private:
	int _fd;
	int _location;
	const Request &_req;
	const ServConfig &_config;

	std::string status;

	std::stringstream _responseHeaders;
	std::stringstream _responseBody;
	std::string _pathObj;

	std::string checkSource();
	void selectLocation();
};

std::string getMimeType(const std::string& file);