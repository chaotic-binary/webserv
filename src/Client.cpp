#include "Client.h"
#include <iostream>
#include <response.h>
#include <stdexcept>
#include <methods.h>

std::vector<std::string> translate_methods(const std::vector<e_methods>& allowed_methods)
{
	std::vector<std::string> ans;

	for(size_t i = 0; i < allowed_methods.size(); i++)
		ans.push_back(ft::to_str(allowed_methods[i]));
	return ans;
}

Response generate_response(const Request &request, const ServConfig &config) {
	const Location& location = config.getLocation(request.getReqTarget());
	if(request.getBody().size() > location.getMaxBody())
		return Response(413);
	const std::vector<e_methods>& allowedMethods= location.getMethods();

	if (allowedMethods.empty() ||
	allowedMethods.end() == find(allowedMethods.begin(), allowedMethods.end(), request.getMethod())) {
		Response response(405);
		response.SetHeader("Allow", translate_methods(allowedMethods));
		return Response(405);;
	}
	try {
		return method_map.at(request.getMethod())(request, config);
	} catch (const std::out_of_range& ex) {
		return Response(501);
	} catch(const RespException &err_rsp)
	{
		return err_rsp.GetRsp();
	} catch(...)
	{
		return Response(500);
	}
}

bool Client::response() {
	if (status_ != READY_TO_SEND)
		return false;

	Response rsp;
	if(req_.isComplete())
		rsp = generate_response(req_, serv_);
	else
		rsp = Response(400);
	std::cout << "============" << rsp.GetCode() << "============" << std::endl ;
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
			//std::cout << "Method: " << ft::to_str(req_.getMethod()) << std::endl;//
		}
	}
	catch (std::exception &e) {
		req_.clear();
		status_ = READY_TO_SEND;
		std::cout << e.what() << std::endl;
	}
}
__deprecated Client::Client(const ServConfig &serv, int fd) : serv_(serv), fd_(fd), status_(READY_TO_READ), req_(fd) {}

Client::~Client() {
	close(fd_);
}

Client::Client(const ServConfig &serv, int fd, const sockaddr_in &clientAddr)
	: serv_(serv), fd_(fd), status_(READY_TO_READ), req_(fd), _clientAddr(clientAddr) {}

