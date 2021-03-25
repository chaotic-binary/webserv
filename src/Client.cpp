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
	if (!request.isComplete())
		return Response(400);
	try {
		const Location &location = config.getLocation(request.getReqTarget());
		const std::vector<e_methods> &allowedMethods = location.getMethods();

		if (request.getBody().size() > location.getMaxBody())
			return Response(413);
		else if (!method_map.count(request.getMethod()))
			return Response(501);
		else if (allowedMethods.empty() ||
			allowedMethods.end() == find(allowedMethods.begin(), allowedMethods.end(), request.getMethod())) {
			Response response(405);
			response.SetHeader("Allow", translate_methods(allowedMethods));
			return response;
		}
		if ((request.getMethod() == PUT || request.getMethod() == POST)
			&& request.getHeader("content-length").empty() && request.getHeader("transfer-encoding") != "chunked")
			return Response(411);
		return method_map.at(request.getMethod())(request, config);
	} catch (const RespException &err_rsp) {
		return err_rsp.GetRsp();
	} catch (...) {
		return Response(500);
	}
}

Response get_response(const Request &request, const ServConfig &config) {
	Response response = generate_response(request, config);
	response.SetDefaultContent(config.getErrorPages());
	if (request.getMethod() == HEAD)
		response.SetBody("");
	return response;
}

void Client::response() {
	if (status_ != READY_TO_SEND && status_ != SENDING)
		return;
	if (status_ == SENDING) {
		this->raw_send();
		return;
	}
	Response rsp = get_response(req_, serv_);
	if (rsp.GetCode() != 200)
		std::cout << "============" << rsp.GetCode() << "============" << std::endl;
	raw_msg = rsp.toString();
	status_ = SENDING;
	next_status = req_.isComplete() ? READY_TO_READ : CLOSE_CONNECTION;
	gettimeofday(&tv_, NULL);
	if (req_.getHeader("connection") == "close")
		next_status = CLOSE_CONNECTION;
	req_.clear();
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
	: serv_(serv), fd_(fd), sended_(0), status_(READY_TO_READ), req_(fd), _clientAddr(clientAddr) {
	gettimeofday(&tv_, NULL);
}

void Client::check() {
	struct timeval cur;
	gettimeofday(&cur, NULL);
	if (cur.tv_sec - tv_.tv_sec > 8000) //TODO: delete magic number
		status_ = CLOSE_CONNECTION;
}

void Client::raw_send() {
	static const size_t MAX_CHUNK_SIZE = pow(2, 20);
	//size_t sended = 0;
	if (sended_ < raw_msg.size()) {
		ssize_t chunk_size = std::min(raw_msg.size() - sended_, MAX_CHUNK_SIZE);
		ssize_t ret = send(fd_, &(raw_msg.c_str()[sended_]), chunk_size, 0); //TODO check send ret
		if (ret <= 0) {
			std::cerr << "ERROR SEND: " << ret << std::endl;
			status_ = CLOSE_CONNECTION;
		}
		else
			sended_ += ret;
	}

	if (sended_ == raw_msg.size()) {
		static int i = 0;
		std::cerr << "File: " << i++ << std::endl;
		sended_ = 0;
		status_ = next_status;
	}
}

e_client_status Client::GetStatus() {
	return status_;
}


