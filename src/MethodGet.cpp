//
// Created by Mahmud Jego on 3/4/21.
//

#include "includes.h"

void methodGet(int fd, const std::string& path, const ServConfig& config)
{
	std::ifstream file(config.getLocations()[0].getRoot() + config.getLocations()[0].getIndex());

	if (!file)
		exit(13);

	std::stringstream response_body;
	response_body << file.rdbuf();

	// Формируем весь ответ вместе с заголовками
	std::stringstream response;
	response << "HTTP/1.1 200 OK\r\n"
			 << "Version: HTTP/1.1\r\n"
			 << "Content-Type: text/html; charset=utf-8\r\n"
			 << "Content-Length: " << response_body.str().length()
			 << "\r\n\r\n"
			 << response_body.str();

	int ret = send(fd, response.str().c_str(), response.str().length(), 0);
	if (ret == -1)
		exit(13);
}

