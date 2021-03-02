#include "Parser.hpp"

int main(int ac, char **av, char **env) {
	if (ac != 2) {
		std::cerr << "Wrong number of arguments: one required\n";
		return (1);
	}
	try {
		Parser p = Parser(av[1]);

		std::cout << p;//test
		std::vector<ServConfig> conf = p.getServs();
		for (size_t i; i < conf.size(); ++i) {
			//conf[i]

		}
		//TODO:next?
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
