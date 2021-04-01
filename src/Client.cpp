#include "Client.h"
#include <iostream>
#include <response.h>
#include <stdexcept>
#include <sys/time.h>
#include "response_generator.h"

bool Client::response() {
	if (status_ != READY_TO_SEND && status_ != SENDING)
		return false;
	if (status_ == SENDING) {
		this->raw_send();
		return true;
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
    return true;
}

int Client::getFd() const {
	return fd_;
}

bool Client::receive() {
	if (status_ != READY_TO_READ)
		return false;
	try {
		if(req_.receive() <= 0)
            return false;
		if (req_.isComplete()) {
			status_ = READY_TO_SEND;
		}
	}
	catch (std::exception &e) {
		req_.clear();
		status_ = READY_TO_SEND;
		std::cout << e.what() << std::endl;
	}
    return true;
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
	if (cur.tv_sec - tv_.tv_sec > 800)
		status_ = CLOSE_CONNECTION;
}

void Client::raw_send() {
	static const size_t MAX_CHUNK_SIZE = pow(2, 20);
	if (sended_ < raw_msg.size()) {
		ssize_t chunk_size = std::min(raw_msg.size() - sended_, MAX_CHUNK_SIZE);
		ssize_t ret = send(fd_, &(raw_msg.c_str()[sended_]), chunk_size, 0);
		if (ret <= 0) {
			//std::cerr << "ERROR SEND: " << ret << std::endl;
			status_ = CLOSE_CONNECTION;
		}
		else {
            gettimeofday(&tv_, NULL);
            sended_ += ret;
        }
	}

	if (sended_ == raw_msg.size()) {
		static int i = 0;
		std::cerr << "Response counter: " << i++ << std::endl;
		sended_ = 0;
		status_ = next_status;
	}
}

e_client_status Client::GetStatus() {
	return status_;
}


