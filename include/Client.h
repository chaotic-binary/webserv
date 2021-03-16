#pragma once

#include "ServConfig.h"
#include "Request.h"

class Client {
public:
	Client(const ServConfig& serv, int fd);
	Client(const ServConfig& serv, int fd, const sockaddr_in& clientAddr);
	~Client();
private:
	const ServConfig &serv_;
	const int fd_;
	enum e_client_status status_;
	Request req_;
	__unused sockaddr_in				_clientAddr;
public:
	void receive();
	bool response();
	void raw_send(const std::string& msg){
		send(fd_, msg.c_str(), msg.length(), 0);
	}
	e_client_status GetStatus() {
		return status_;
	}
	const ServConfig &getServ() const;
	int getFd() const;
};
