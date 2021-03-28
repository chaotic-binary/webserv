#include "Server.h"
#include "base64.h"

volatile bool loop = true;

int main(int ac, char **av)
{
    std::cout <<"Encoded: " << base64_encode("HELLON") << std::endl;
    std::cout <<"Encoded: " << base64_encode("HELLONA") << std::endl;
    std::cout <<"Encoded: " << base64_encode("HELLONAB") << std::endl;
    std::cout <<"Encoded: " << base64_encode("HELLONABC") << std::endl;

	if (ac < 2)
		return 1;
	try {
		Server server(Parser().parse(av[1]));
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
