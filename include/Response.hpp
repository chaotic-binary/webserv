//
// Created by Mahmud Jego on 3/4/21.
//

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "ServConfig.hpp"
#include "Request.h"

class Response {
public:
	Response(int fd, const Request &req, const ServConfig& config);
private:
	int _fd;
	int _location;
	const Request &_req;
	const ServConfig &_config;

	int checkSource();
	int selectLocation();
};


#endif //RESPONSE_HPP
