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
		int ret = server.Select();
		if ( ret == -1 )
			throw Server::Error("select SUPER MSG");
		if ( ret == 0 ) {
			continue ;
		}
		server.checkSockets();
		server.checkClients();
	}
//	TODO:: close all sockets;
	return (0);
}
