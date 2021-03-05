#include "Client.h"
#include <iostream>
#include <unistd.h>
#include "methods.h"

std::string generate_response(const Request& request, const ServConfig &config) {
	std::cout << "tossern" << std::endl;
	// for test
	char msg[] = "hello from server\n";

	std::stringstream response_body;
	response_body << "<title>Test C++ HTTP Server</title>\n"
				  << "<h1>Test page</h1>\n"
				  << "<p>This is body of the test page...</p>\n"
				  << "<h2>Request headers</h2>\n"
				  << "<pre>" << msg << "</pre>\n"
				  << "<em><small>Test C++ Http Server</small></em>\n";

	// Формируем весь ответ вместе с заголовками
	std::stringstream response;
	response << "HTTP/1.1 200 OK\r\n"
			 << "Version: HTTP/1.1\r\n"
			 << "Content-Type: text/html; charset=utf-8\r\n"
			 << "Content-Length: " << response_body.str().length()
			 << "\r\n\r\n"
			 << response_body.str();
	// for test

	//if (ret == -1)
	//	throw Error("send message");
	return response.str();
}

bool Client::response() {
	if (status_ == READY_TO_READ)
		return false;
	if(req_.getMethod() == GET)
		methodGet(fd_,req_.getReqTarget(), serv_);
	else {
		std::string response = generate_response(req_, serv_);
		int ret = send(fd_, response.c_str(), response.length(), 0);
	}
	//TODO: Do we need it? bzero(this->_buffer, 2048);
	//int ret = send(fd_, response.c_str(), response.length(), 0);
	return status_ == CLOSE_CONNECTION;
}

const ServConfig &Client::getServ() const {
	return serv_;
}

int Client::getFd() const {
	return fd_;
}

void Client::receive() {
	if (status_ != READY_TO_READ)
		return;
	try {
		int ret;
		char buffer[2049];
		std::string raw_request;
		//TODO: MOVE TO REQUEST
		while ((ret = read(fd_, buffer, 2048)) > 0) {
			buffer[ret] = 0x0;
			raw_request += buffer;
		}
		req_ = Request(raw_request);
		std::cout << "<REQUEST\n" << req_ << std::endl;
		std::cout << "REQUEST>\n"; //test
		status_ = CLOSE_CONNECTION;
	}
	catch (...) {
		std::cout << "TODO: handle the half msg!!!" << std::endl;
	}
}

Client::Client(const ServConfig &serv, int fd) : serv_(serv), fd_(fd), status_(READY_TO_READ), req_("")
{}

Client::~Client() {
	close(fd_);
}

