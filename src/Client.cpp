#include "Client.h"
#include <iostream>
#include <response.h>
#include <stdexcept>
#include <methods.h>
#include <sys/time.h>

std::vector<std::string> translate_methods(const std::vector<e_methods> &allowed_methods) {
	std::vector<std::string> ans;

	for (size_t i = 0; i < allowed_methods.size(); i++)
		ans.push_back(ft::to_str(allowed_methods[i]));
	return ans;
}

Response generate_response(const Request &request, const ServConfig &config) {
	try {
		const Location &location = config.getLocation(request.getReqTarget());
		if (request.getBody().size() > location.getMaxBody())
			return Response(413);
		if (!method_map.count(request.getMethod()))
			return Response(501);
		const std::vector<e_methods> &allowedMethods = location.getMethods();
		if (allowedMethods.empty() ||
			allowedMethods.end() == find(allowedMethods.begin(), allowedMethods.end(), request.getMethod())) {
			Response response(405);
			response.SetHeader("Allow", translate_methods(allowedMethods));
			return response;
		}
		return method_map.at(request.getMethod())(request, config);
	} catch (const RespException &err_rsp) {
		return err_rsp.GetRsp();
	} catch (...) {
		return Response(500);
	}
}

bool Client::response() {
	if (status_ != READY_TO_SEND)
		return false;

	Response rsp;
	if (req_.isComplete())
		rsp = generate_response(req_, serv_);
	else {
		rsp = Response(400);
	}
	if(rsp.GetCode() != 200)
		std::cout << "============" << rsp.GetCode() << "============" << std::endl;
	this->raw_send(rsp.Generate());
	status_ = req_.isComplete() ? READY_TO_READ : CLOSE_CONNECTION;
	gettimeofday(&tv_, NULL);
	if (req_.getHeader("connection") == "close")
		status_ = CLOSE_CONNECTION;
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
			//std::cout << "<REQUEST\n" << req_ << std::endl;
			//std::cout << "REQUEST>\n"; //test
			//std::cout << "Method: " << ft::to_str(req_.getMethod()) << std::endl;//
		}
	}
	catch (std::exception &e) {
		req_.clear();
		status_ = READY_TO_SEND;
		std::cout << e.what() << std::endl;
	}
}

Client::~Client() {
	close(fd_);
}

Client::Client(const ServConfig &serv, int fd, const sockaddr_in &clientAddr)
	: serv_(serv), fd_(fd), status_(READY_TO_READ), req_(fd), _clientAddr(clientAddr) {
	gettimeofday(&tv_, NULL);
}

void Client::check() {
	struct timeval cur;
	gettimeofday(&cur, NULL);
	if(cur.tv_sec  - tv_.tv_sec > 120) //TODO: delete magic number
		status_ = CLOSE_CONNECTION;
}

void Client::raw_send(const std::string &msg) const {
	static const size_t MAX_CHUNK_SIZE = pow(2, 20);
	size_t sended = 0;
	if (sended < msg.size()) {
		ssize_t chunk_size = std::min(msg.size() - sended, MAX_CHUNK_SIZE);
		ssize_t ret = send(fd_, &(msg.c_str()[sended]), chunk_size, 0); //TODO check send ret
		if(ret <= 0)
			std::cerr << "ERROR SEND: " << ret << std::endl;
		else
			sended += ret;
	}
}

e_client_status Client::GetStatus() {
	return status_;
}


