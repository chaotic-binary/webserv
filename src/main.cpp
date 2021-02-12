#include "../include/Parser.hpp"

int main(int ac, char **av, char **env)
{
	if (ac != 2) {
		std::cerr  << "Wrong number of arguments: one required\n";
		return (1);
	}
	try {
		Parser p = Parser(av[1]);
		std::cout << p;//test
	} catch(const std::exception& e) {
		std::cerr  << e.what() << std::endl;
	}
}
