#include "Server.h"
volatile bool loop = true;

int main(int ac, char **av)
{
	if (ac < 2)
		return 1;
	try {
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
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
//	TODO:: close all sockets;
	return (0);
}
