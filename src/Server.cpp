//
// Created by Mahmud Jego on 2/25/21.
//

#include "Server.hpp"
#include "Request.h"
#include "Client.h"

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


__deprecated void Server::toSend(const int fd)
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
		throw Error("send message");
	//close(fd);
	// fd = -1;
}

void Server::newClient(int indexServer)
{
	int addrlen = sizeof(sockaddr);

	int connection = accept(this->_servers[indexServer].getSockFd(),
							(struct sockaddr *) &this->_servers[indexServer].getSockAddr(),
							(socklen_t *) &addrlen);
	if (connection == -1)
		throw Error("connection"); //TODO: move it in client?
	_clients.push_back(SharedPtr<Client>(new Client(_servers[indexServer], connection)));
	fcntl(connection, F_SETFL, O_NONBLOCK);
}

int Server::getMaxSockFd() const
{
	int maxFd = -1;
	for (size_t i = 0; i < this->_amountServers; ++i)
		maxFd = std::max(_servers[i].getSockFd(), maxFd);

	for (size_t i = 0; i < getClients().size(); ++i)
		maxFd = std::max(getClients()[i]->getFd(), maxFd);

	if (maxFd == -1)
		throw Error("get Max Fd");
	return (maxFd);
}

void Server::reloadFdSets()
{
	FD_ZERO(&_readFds); // clean set TODO:why?
	FD_ZERO(&_writeFds); // clean set
	std::vector<SharedPtr<Client> >::const_iterator client;
	for (client = getClients().cbegin(); client != getClients().cend(); client++) {
		FD_SET((*client)->getFd(), &_readFds);
		FD_SET((*client)->getFd(), &_writeFds);
	}
	for (size_t i = 0; i < _servers.size(); ++i)
		FD_SET(_servers[i].getSockFd(), &_readFds); //TODO add writeFds if not working
}

void Server::checkClientsAfter()
{
	std::vector< SharedPtr<Client> >::iterator it;
	for (it = _clients.begin(); it != _clients.end();)
	{
		if (FD_ISSET((*it)->getFd(), &_readFds))
			(*it)->receive();

		if (FD_ISSET((*it)->getFd(), &_writeFds) && (*it)->response())
			_clients.erase(it);
		else
			++it;
	}
}

int Server::Select()
{
	struct timeval tv;
	tv.tv_sec = 10;
	tv.tv_usec = 0;
	reloadFdSets();
	return (select(getMaxSockFd() + 1, &_readFds, &_writeFds, NULL, &tv));
}

void Server::checkSockets()
{
	for (size_t i = 0; i < _servers.size(); ++i)
	{
		if (FD_ISSET(this->_servers[i].getSockFd(), &_readFds))
		{
			this->newClient(i);
		}
	}
}

void Server::sendCgi(const Request &request)
{
	Cgi cgi(request);
}

const std::vector<SharedPtr<Client> > & Server::getClients() const {
	return _clients;
}

