//
// Created by Mahmud Jego on 2/25/21.
//

#ifndef SERVER_HPP
#define SERVER_HPP

#include "includes.h"
#include "Parser.h"
#include "Cgi.h"
#include "Client.h"
#include "SharedPtr.h"

class Server {
public:
	Server(char *);
	Server(const std::vector<ServConfig>& servers);

	void												initSockets();
	//void												initHeaders(const std::string& headers);

	void												checkClients();
	void												checkSockets();
	int													Select();
	int													getMaxSockFd() const;
	void												newClient(int indexServer);


	void												sendCgi(const Request &request);

	class Error: public std::logic_error {
	private: Error();
	public: Error(std::string s) : std::logic_error("Error: " + s + ": " + strerror(errno)) {};
	};

private:
	std::vector<ServConfig>								_servers;
	const size_t										_amountServers;
//	std::vector<int>									_clientsFd;
	std::vector<SharedPtr<Client> >						_clients;
	char												_buffer[2048];
	fd_set												_readFds;
	fd_set												_writeFds;

	void												reloadFdSets();
	const std::vector<SharedPtr<Client> > & getClients() const;
};

#endif //SERVER_HPP
