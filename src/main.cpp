#include "Server.h"
volatile bool loop = true;

int main(int ac, char **av)
{
	if (ac < 2)
		return 1;

	Server server(Parser(av[1]).getServs());
	server.initSockets();
	while (loop)
	{
		//FD_SET(server.getMaxSockFd(), &readFds); //TODO: WHY?!
		int ret = server.Select();
//		std::cout << "wait select" << std::endl;
		if ( ret == -1 )
			throw Server::Error("select");
		if ( ret == 0 )
		{
			continue ;
		}
		server.checkSockets();
		server.checkClients();
	}
//	TODO:: close all sockets;
	return (0);
}
