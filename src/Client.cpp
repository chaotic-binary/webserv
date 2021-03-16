#include "Client.h"
#include <iostream>
#include <response.h>
#include <stdexcept>
#include <fstream>
#include <methods.h>


Response generate_response(const Request &request, const ServConfig &config) {
	try {
		return method_map.at(request.getMethod())(request, config);
	} catch (const std::out_of_range& ex) {
		return Response(405);
	} catch(const RespException &err_rsp)
	{
		return err_rsp.GetRsp();
	}
}

bool Client::response() {
	if (status_ != READY_TO_SEND)
		return false;

	Response rsp = generate_response(req_, serv_);

	this->raw_send(rsp.Generate());
	status_ = READY_TO_READ;
	try {
		if (req_.getHeaders().at("connection") == "close")
			status_ = CLOSE_CONNECTION;
	} catch (const std::out_of_range &) {}
	req_.clear();
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
		req_.receive();
		if (req_.isComplete()) {
			status_ = READY_TO_SEND;
			std::cout << "<REQUEST\n" << req_ << std::endl;
			std::cout << "REQUEST>\n"; //test
		}
	}
	catch (std::exception &) {
		std::cout << "TODO: handle the half msg!!!" << std::endl;
	}
}
__deprecated Client::Client(const ServConfig &serv, int fd) : serv_(serv), fd_(fd), status_(READY_TO_READ), req_(fd) {}

Client::~Client() {
	close(fd_);
}

Client::Client(const ServConfig &serv, int fd, const sockaddr_in &clientAddr)
	: serv_(serv), fd_(fd), status_(READY_TO_READ), req_(fd), _clientAddr(clientAddr) {}

