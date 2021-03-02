//
// Created by Mahmud Jego on 2/25/21.
//

#include "Server.hpp"

Server::Server(char *config)
{
	std::cout << "parser config" << std::endl;
	Parser parser(config);
	this->_servers = parser.getServs();
	for (int i = 0; i < this->_servers.size(); ++i)
	{
		sockaddr_in tmp;

		tmp.sin_family = AF_INET; // ipV4
		tmp.sin_port = htons(this->_servers[i].getPort());
		tmp.sin_addr.s_addr = inet_addr(this->_servers[i].getHost().c_str());
		this->_servers[i].setSockAddr(tmp);
		this->_servers[i].setSockFd(-1);
	}
}

Server::Server(const std::string &ip, int port)
{}

Server::Server(const std::vector<ServConfig>& servers)
: _servers(servers)
{
	for (int i = 0; i < this->_servers.size(); ++i)
	{
		sockaddr_in tmp;

		tmp.sin_family = AF_INET; // ipV4
		tmp.sin_port = htons(this->_servers[i].getPort());
		tmp.sin_addr.s_addr = inet_addr(this->_servers[i].getHost().c_str());
		this->_servers[i].setSockAddr(tmp);
		this->_servers[i].setSockFd(-1);
	}
}

int Server::getSocket()
{
	this->_sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_sockFd == -1)
	{
		std::cout << "error: create socket, errno: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	int opt = 1;
	if (setsockopt(this->_sockFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		std::cout << "error: setsockopt, errno: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	if (bind(this->_sockFd, (struct sockaddr *) &this->_sockAddr, sizeof(sockaddr)) == -1)
	{
		std::cout << "error: bind, maybe port busy, errno: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	if (listen(this->_sockFd, 10) < 0)
	{
		std::cout << "error: listen on socket. errno: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	return (this->_sockFd);
}

void Server::receive(int fd)
{
	std::cout << "revs" << std::endl;
	int ret = recv(fd, this->_buffer, 2048, MSG_PEEK);
	if (ret == -1)
	{
		std::cout << "error: connection. errno: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
/*	if (ret < 2048)

	std::cout << this->_buffer << std::endl;*/
}

void Server::toSend(int& fd)
{
	std::cout << "tossern" << std::endl;
	// for test
	char msg[] = "hello from server\n";

	std::stringstream response_body;
	response_body << "<title>Test C++ HTTP Server</title>\n"
				  << "<h1>Test page</h1>\n"
				  << "<p>This is body of the test page...</p>\n"
				  << "<h2>Request headers</h2>\n"
				  << "<pre>" << msg << "</pre>\n"
				  << "<em><small>Test C++ Http Server</small></em>\n";

	// Формируем весь ответ вместе с заголовками
	std::stringstream response;
	response << "HTTP/1.1 200 OK\r\n"
			 << "Version: HTTP/1.1\r\n"
			 << "Content-Type: text/html; charset=utf-8\r\n"
			 << "Content-Length: " << response_body.str().length()
			 << "\r\n\r\n"
			 << response_body.str();
	// for test
	bzero(this->_buffer, 2048);
	// Send a message to the connection
	// write
	int ret = send(fd, response.str().c_str(), response.str().length(), 0);
	if (ret == -1)
	{
		std::cout << "error: send message. errno: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	close(fd);
	fd = -1;
}

void Server::newClient()
{
	int addrlen = sizeof(sockaddr);

	int connection = accept(this->_sockFd, (struct sockaddr *) &this->_sockAddr, (socklen_t *) &addrlen);
	if (connection == -1)
	{
		std::cout << "error: connection. errno: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	this->_clientsFd.push_back(connection);
	fcntl(connection, F_SETFL, O_NONBLOCK);
}

int Server::getSockFd() const {
	return (this->_sockFd);
}
void Server::checkClientsBefore(fd_set &readFds, fd_set &writeFds, int &max_d)
{
	for (int i = 0; i < this->_clientsFd.size(); ++i)
	{
		if (this->_clientsFd[i] != -1)
		{
			FD_SET(this->_clientsFd[i], &readFds);
		}
		/*if (data)
		{
			FD_SET(this->_clientsFd[i], &writeFds);
		}*/
		if (max_d < this->_clientsFd[i])
		{
			max_d = this->_clientsFd[i];
		}
	}
}

void Server::checkClientsAfter(fd_set &readFds, fd_set &writeFds, int &max_d)
{
	for (int i = 0; i < this->_clientsFd.size(); ++i)
	{
		if (FD_ISSET(this->_clientsFd[i], &readFds))
		{
			this->receive(this->_clientsFd[i]);
		}
		if (FD_ISSET(this->_clientsFd[i], &writeFds))
		{
			this->toSend(this->_clientsFd[i]);
		}
	}
	std::cout << "vyh" << std::endl;
}

int Server::Select(fd_set &readFds, fd_set &writeFds, int &max_d) const
{
	struct timeval tv;
	tv.tv_sec = 10;
	tv.tv_usec = 0;

	return (select(max_d + 1, &readFds, &writeFds, NULL, &tv ));
}

