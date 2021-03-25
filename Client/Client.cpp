//
// Created by Mahmud Jego on 2/24/21.
//

#include "Client.hpp"

Client::Client(const std::string& ip, short port, const std::string& target)
{
	this->_sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
	this->_sockaddr.sin_family = AF_INET;
	this->_sockaddr.sin_port = htons(port);
	this->_msg = "GET " + target + " HTTP/1.1\r\nHost: 127.0.0.1:3456\r\nConnection: close\r\nCache-Control: max-age=0\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/88.0.4324.41 YaBrowser/21.2.0.2458 Yowser/2.5 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\nSec-Fetch-Site: none\r\nSec-Fetch-Mode: navigate\r\nSec-Fetch-User: ?1\r\nSec-Fetch-Dest: document\r\nAccept-Encoding: gzip, deflate, br\r\nAccept-Language: ru,en;q=0.9\r\n\r\n";
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
	char *buff[10000];
	while (recv(conn, buff, 9999, 0 ) > 0)
	{
		std::cout << *buff << std::endl;
	}
	close(this->_sockfd);
}
