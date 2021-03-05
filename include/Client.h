#pragma once
#include "ServConfig.hpp"
#include "Request.h"

enum e_client_status
{
	CLOSE_CONNECTION, READY_TO_READ, READY_TO_SEND
};


std::string generate_responce(Request request, const ServConfig &config);

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
