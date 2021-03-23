#pragma once

#include "ServConfig.h"
#include "Request.h"
#include <sys/time.h>

class Client {
public:
	Client(const ServConfig& serv, int fd);
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
	void raw_send(const std::string& msg){
		size_t sended = 0;
		while (sended < msg.size())
			if(msg.size() - sended < 1024)
				sended += send(fd_, &(msg.c_str()[sended]), msg.size() - sended,0); //TODO check send ret
			else
				sended += send(fd_, &(msg.c_str()[sended]), 1024,0); //TODO check send ret
	}
	void check()
	{
		struct timeval cur;
		gettimeofday(&cur, NULL);
		if(cur.tv_sec  - tv_.tv_sec > 120) //TODO: delete magic number
			status_ = CLOSE_CONNECTION;
	}
	e_client_status GetStatus() {
		return status_;
	}
	const ServConfig &getServ() const;
	int getFd() const;
};
