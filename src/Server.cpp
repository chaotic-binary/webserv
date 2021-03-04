//
// Created by Mahmud Jego on 2/25/21.
//

#include "Server.hpp"
#include "Request.h"

Server::Server(char *config)
: _servers(Parser(config).getServs()), _amountServers(_servers.size())
{
	std::cout << "parser config" << std::endl;
	for (size_t i = 0; i < this->_amountServers; ++i)
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
: _amountServers(1)
{}

Server::Server(const std::vector<ServConfig>& servers)
: _servers(servers), _amountServers(servers.size())
{
	for (size_t i = 0; i < this->_amountServers; ++i)
	{
		sockaddr_in tmp;

		tmp.sin_family = AF_INET; // ipV4
		tmp.sin_port = htons(this->_servers[i].getPort());
		tmp.sin_addr.s_addr = inet_addr(this->_servers[i].getHost().c_str());
		this->_servers[i].setSockAddr(tmp);
		this->_servers[i].setSockFd(-1);
	}
}

void Server::initSockets()
{
	for (size_t i = 0; i < this->_amountServers; ++i)
	{
		this->_servers[i].setSockFd(socket(AF_INET, SOCK_STREAM, 0));
		if (this->_servers[i].getSockFd() == -1)
			throw Error("create socket");
		int opt = 1;
		if (setsockopt(this->_servers[i].getSockFd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
			throw Error("setsockopt");
		if (bind(this->_servers[i].getSockFd(), (struct sockaddr *) &this->_servers[i].getSockAddr(), sizeof(sockaddr)) == -1)
			throw Error("bind, maybe port busy");
		if (listen(this->_servers[i].getSockFd(), 10) < 0)
			throw Error("listen on socket");
	}
}

bool isFinished(const std::string &s)
{
	static int		bodyPresent = 0;
	static size_t	contentLength = 0;
	size_t	i;

	if ((i = s.find("Content-Length:")) != s.npos)
	{
		bodyPresent = 1;
		if ((i = i + 16) < s.size())
			contentLength = ft::to_num(&s[i]);
	}
	if (s.find("Transfer-Encoding: chunked") != s.npos)
		bodyPresent = 2;
	if ((i = s.find("\r\n\r\n")))
	{
		if (!bodyPresent)
			return true;
		if (bodyPresent == 1)
		{
			if (s.size() - i == contentLength + 4)
				return true;
		}
		if (bodyPresent == 2 && i < s.size() && s.find("0\r\n\r\n", i) != s.npos)
			return true;
	}
	return false;
}

void Server::receive(int fd)
{
	std::cout << "revs" << std::endl;
	int ret;
	std::string headers;

//	while ((ret = recv(fd, this->_buffer, 2048, MSG_PEEK)) > 0)
	//size_t begin = 0;
	while ((ret = read(fd, this->_buffer, 2048)) > 0)
	{
		this->_buffer[ret] = 0x0;
		//begin = ret;
		headers += this->_buffer;
	}
/*	if (ret == -1)
	{
		this->toSend(fd);// temporarily
		std::cout << "error: read. errno: " << strerror(errno) << std::endl; // FORBIDDEN TO USE!!!!!!!!!!!!!
		exit(EXIT_FAILURE);
	}*/
	std::cout << headers << std::endl;

	if (isFinished(headers))
	{
		Request request(headers);
		std::cout << "<REQUEST\n" << request << std::endl;
		std::cout << "REQUEST>\n"; //test
		//if (request.getMethod() == POST)
			//this->sendCgi(request);
		//if (request.getMethod() == GET)
		methodGet(fd, request.getReqTarget(), this->_servers[3]);
		close(fd); // temporarily
	}

}

void Server::toSend(int& fd)
{

}

void Server::newClient(int indexServer)
{
	int addrlen = sizeof(sockaddr);

	int connection = accept(this->_servers[indexServer].getSockFd(),
							(struct sockaddr *) &this->_servers[indexServer].getSockAddr(),
							(socklen_t *) &addrlen);
	if (connection == -1)
		throw Error("connection");
	this->_clientsFd.push_back(connection);
	fcntl(connection, F_SETFL, O_NONBLOCK);
}

int Server::getMaxSockFd() const
{
	int maxFd = -1;
	for (size_t i = 0; i < this->_amountServers; ++i)
		maxFd = std::max(_servers[i].getSockFd(), maxFd);

	for (size_t i = 0; i <_clientsFd.size(); ++i)
		maxFd = std::max(_clientsFd[i], maxFd);

	if (maxFd == -1)
		throw Error("get Max Fd");
	return (maxFd);
}
void Server::checkClientsBefore(fd_set &readFds, fd_set &writeFds)
{
	for (size_t i = 0; i < this->_clientsFd.size(); ++i)
	{
		FD_SET(this->_clientsFd[i], &readFds);
	}
	for (size_t i = 0; i < _servers.size(); ++i)
	{
		FD_SET(_servers[i].getSockFd(), &readFds);
	}
}

void Server::checkClientsAfter(fd_set &readFds, fd_set &writeFds)
{
	std::vector<int>::iterator pos = this->_clientsFd.begin();

	for (size_t i = 0; i < this->_clientsFd.size(); ++i) {
		if (FD_ISSET(this->_clientsFd[i], &readFds)) {
			this->receive(this->_clientsFd[i]);
			//temporarily part
			{
				this->_clientsFd.erase(pos + i); //TODO: remove index error
			}
		}
		if (FD_ISSET(this->_clientsFd[i], &writeFds))
		{
			this->toSend(this->_clientsFd[i]);
		}
	}
}

int Server::Select(fd_set &readFds, fd_set &writeFds) const
{
	struct timeval tv;
	tv.tv_sec = 10;
	tv.tv_usec = 0;

	return (select(getMaxSockFd() + 1, &readFds, &writeFds, NULL, &tv));
}

void Server::checkSockets(fd_set &readFds, fd_set &writeFds)
{
	for (size_t i = 0; i < this->_amountServers; ++i)
	{
		if (FD_ISSET(this->_servers[i].getSockFd(), &readFds))
		{
			this->newClient(i);
		}
	}
}

void Server::sendCgi(const Request &request)
{
	Cgi cgi(request);
}

