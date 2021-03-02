//
// Created by Mahmud Jego on 2/24/21.
//

#include "Client.hpp"

Client::Client(const std::string& ip, short port)
{
	this->_sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
	this->_sockaddr.sin_family = AF_INET;
	this->_sockaddr.sin_port = htons(port);
	this->_msg = "hello from TCP\n";
}

void Client::getSocketFd()
{
	this->_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockfd == -1)
	{
		std::cerr << "error: socket fd" << std::endl;
	}
}

void Client::start()
{
	this->getSocketFd();
	int conn = connect(this->_sockfd, (struct sockaddr*) &this->_sockaddr, sizeof(sockaddr_in));
	if (conn == -1)
		std::cerr << "error: connect" << std::endl;
	int sizeBuff = send(this->_sockfd, this->_msg.c_str(), this->_msg.size(), 0);
	if (sizeBuff == -1)
	{
		std::cerr << "error: send" << std::endl;
	}
	close(this->_sockfd);
}
