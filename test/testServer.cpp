//
// Created by Mahmud Jego on 3/2/21.
//

#include "Server.hpp"
volatile bool loop = true;

int main(int ac, char **av)
{
	Server server(Parser(av[1]).getServs());
//	Server serverTestParser(av[1]);
//	Server server("127.0.0.1", 8000);
	int max_d = 1654641;

	while (loop)
	{
		fd_set readFds, writeFds;
		FD_ZERO(&readFds); // clean set
		FD_ZERO(&writeFds); // clean set
		FD_SET(server.getSockFd(), &readFds);

		server.checkClientsBefore(readFds, writeFds, max_d);
		int ret = server.Select(readFds, writeFds, max_d);
		std::cout << "tut1" << std::endl;
		if ( ret == -1 )
		{
			std::cout << "error: select. errno: " << strerror(errno) << std::endl;
			exit(EXIT_FAILURE);
		}
		if ( ret == 0 )
		{
			continue ;
		}
		if (FD_ISSET(server.getSockFd(), &readFds))
		{
			std::cout << "tut2" << std::endl;
//			server.newClient();
		}
		server.checkClientsAfter(readFds, writeFds, max_d);
	}
//	close(server.initSockets());
	return (0);
}
