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

	void												initSockets();
	void												initHeaders(const std::string& headers);

	void												checkClientsBefore(fd_set& readFds, fd_set& writeFds, int& max_d);
	void												checkClientsAfter(fd_set& readFds, fd_set& writeFds, int& max_d);
	void												checkSockets(fd_set& readFds, fd_set& writeFds, int& max_d);

	int													Select(fd_set& readFds, fd_set& writeFds, int& max_d) const;
	int													getMaxSockFd();
	void												newClient(int indexServer);

	void												toSend(int&);
	void												receive(int);

private:
	std::vector<ServConfig>								_servers;
	const size_t										_amountServers;
	std::vector<int>									_clientsFd;
	char												_buffer[2048];
	std::map<std::string, std::vector<std::string> >	_headers;
};


#endif //SERVER_HPP
