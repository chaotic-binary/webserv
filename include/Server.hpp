//
// Created by Mahmud Jego on 2/25/21.
//

#ifndef SERVER_HPP
#define SERVER_HPP

#include "includes.h"
#include "Parser.hpp"

class Server {
public:
	Server(char *);
	Server(const std::vector<ServConfig>& servers);
	Server(const std::string& ip, int port);
	void initSockets();

	void checkClientsBefore(fd_set& readFds, fd_set& writeFds, int& max_d);
	void checkClientsAfter(fd_set& readFds, fd_set& writeFds, int& max_d);

	int Select(fd_set& readFds, fd_set& writeFds, int& max_d) const;
	int getSockFd() const;
	void newClient(int &sockFd, sockaddr_in& sockAddr);

	void toSend(int&);
	void receive(int);

private:
	std::vector<ServConfig> _servers;

	std::vector<int> _clientsFd;
	char _buffer[2048];
};


#endif //SERVER_HPP
