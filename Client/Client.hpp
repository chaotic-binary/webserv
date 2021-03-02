//
// Created by Mahmud Jego on 2/24/21.
//

#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Client {
public:
	Client(const std::string& ip, short port);
	void start();
private:
	int _sockfd;
//	short _port;
	sockaddr_in _sockaddr;
	std::string _buff;
	std::string _msg;

	void getSocketFd();
};


#endif //CLIENT_HPP
