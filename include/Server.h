#pragma once

#include "Parser.h"
#include "Cgi.h"
#include "Client.h"
#include "SharedPtr.h"
#include <stdexcept>

class Server {
 public:
	explicit Server(const std::vector<ServConfig> &servers);
	void initSockets();
	void checkClients();
	void checkSockets();
	int Select();
	int getMaxSockFd() const;
	void newClient(int indexServer);

	class Error : public std::runtime_error {
	 private:
		Error();
	 public:
		explicit Error(const std::string &msg) :
		std::runtime_error("Error: " + msg + ": " + strerror(errno)) {};
	};
 private:
	Server();
	Server(const Server &);
	void operator=(const Server &);

	std::vector<ServConfig> _servers;
	const size_t _amountServers;
	std::vector<SharedPtr<Client> > _clients;
	fd_set _readFds;
	fd_set _writeFds;

	void reloadFdSets();
	const std::vector<SharedPtr<Client> > &getClients() const;
};
