//
// Created by Mahmud Jego on 3/2/21.
//

#include "Server.hpp"
volatile bool loop = true;

int main(int ac, char **av, char **env)
{
	try {
		Server server(Parser(av[1]).getServs());
		server.initSockets();
		while (loop)
		{
			int ret = server.Select();
			std::cout << "wait select" << std::endl;
			if ( ret == -1 )
				throw Server::Error("select");
			server.checkSockets();
			server.checkClients();
		}
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	return (0);
}
