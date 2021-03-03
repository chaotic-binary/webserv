//
// Created by Mahmud Jego on 3/2/21.
//

#include "Server.hpp"
volatile bool loop = true;

int main(int ac, char **av, char **env)
{
	Server server(Parser(av[1]).getServs());
	server.initSockets();
//	Server serverTestParser(av[1]);
//	Server server("127.0.0.1", 8000);

	while (loop)
	{
		fd_set readFds, writeFds;
		FD_ZERO(&readFds); // clean set
		FD_ZERO(&writeFds); // clean set
		FD_SET(server.getMaxSockFd(), &readFds);

		server.checkClientsBefore(readFds, writeFds);
		int ret = server.Select(readFds, writeFds);
		std::cout << "wait select" << std::endl;
		if ( ret == -1 )
			throw Server::Error("select");
		/*{

			std::cout << "error: select. errno: " << strerror(errno) << std::endl;
			exit(EXIT_FAILURE);
		}*/
		/*if ( ret == 0 )
		{
			continue ;
		}*/
		server.checkSockets(readFds, writeFds);
		server.checkClientsAfter(readFds, writeFds);
	}
//	close(server.initSockets());
	return (0);
}
