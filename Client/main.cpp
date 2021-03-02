//
// Created by Mahmud Jego on 2/24/21.
//

#include "Client.hpp"

int main(int ac, char **av)
{
	if (ac < 2)
	{
		std::cerr << "no arg" << std::endl;
	}
	int port = atoi(av[1]);
	int count = atoi(av[2]);
	for (int i = 0; i < count; ++i)
	{
		Client client("127.0.0.1", port++);
		client.start();
	}
}
