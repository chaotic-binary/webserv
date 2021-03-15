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
	const ServConfig &getServ() const;
	int getFd() const;
};
