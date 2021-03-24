#include <fcntl.h>
#include "Server.h"
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
		if (listen(this->_servers[i].getSockFd(), -1) < 0)
			throw Error("listen on socket");
	}
}

void Server::newClient(int indexServer)
{
	sockaddr_in		clientAddr;
	int addrlen = sizeof(sockaddr);

	int connection = accept(this->_servers[indexServer].getSockFd(),
					(struct sockaddr *) &clientAddr,
					(socklen_t *) &addrlen);
	if (connection == -1)
		throw Error("connection"); //TODO: move it in client?
	_clients.push_back(SharedPtr<Client>(new Client(_servers[indexServer], connection, clientAddr)));
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
	FD_ZERO(&_readFds);
	FD_ZERO(&_writeFds);
	std::vector<SharedPtr<Client> >::const_iterator client;
	for (client = getClients().cbegin(); client != getClients().cend(); client++) {
		FD_SET((*client)->getFd(), &_readFds);
		FD_SET((*client)->getFd(), &_writeFds);
	}
	for (size_t i = 0; i < _servers.size(); ++i)
		FD_SET(_servers[i].getSockFd(), &_readFds); //TODO add writeFds if not working
}

void Server::checkClients()
{
	std::vector< SharedPtr<Client> >::iterator it;
	for (it = _clients.begin(); it != _clients.end();)
	{
		if (FD_ISSET((*it)->getFd(), &_readFds) &&
			(*it)->GetStatus() == READY_TO_READ)
			(*it)->receive();
		else if(FD_ISSET((*it)->getFd(), &_writeFds))
			(*it)->response();

		(*it)->check();

		if((*it)->GetStatus() == CLOSE_CONNECTION)
			_clients.erase(it);
		else
			++it;
	}
}

int Server::Select()
{
	reloadFdSets();
	struct timeval tv = {10, 0};
	return (select(getMaxSockFd() + 1, &_readFds, &_writeFds, NULL, &tv));
}

void Server::checkSockets()
{
	for (size_t i = 0; i < _servers.size(); ++i) {
		if (FD_ISSET(this->_servers[i].getSockFd(), &_readFds))
			this->newClient(i);
	}
}

//void Server::sendCgi(const Request &request)
//{
//	Cgi cgi(request);
//}

const std::vector<SharedPtr<Client> > & Server::getClients() const {
	return _clients;
}

