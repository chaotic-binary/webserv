#pragma once
#include "ServConfig.h"
#include "Request.h"

class Client {
public:
	Client(const ServConfig& serv, int fd);
	~Client();
private:
	const ServConfig &serv_;
	const int fd_;
	enum e_client_status status_;
	Request req_;
public:
	void receive();
	bool response();
	const ServConfig &getServ() const;
	int getFd() const;
};
