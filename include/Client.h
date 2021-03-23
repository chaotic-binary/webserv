#pragma once

#include "ServConfig.h"
#include "Request.h"
#include <sys/time.h>
#include <cmath>

class Client {
public:
	Client(const ServConfig& serv, int fd, const sockaddr_in& clientAddr);
	~Client();
private:
	const ServConfig &serv_;
	const int fd_;
	enum e_client_status status_;
	struct timeval tv_;
	Request req_;
	__unused sockaddr_in				_clientAddr;
public:
	void receive();
	bool response();
	void raw_send(const std::string& msg) const;
	void check();
	e_client_status GetStatus();
	const ServConfig &getServ() const;
	int getFd() const;
};
