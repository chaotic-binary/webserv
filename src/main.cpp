#include "Parser.hpp"

int main(int ac, char **av, char **env) {
	/*if (ac != 2) {
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
	}*/
	std::string raw_request = "8ab";
	std::string n = raw_request.substr(0, raw_request.size() - 2);
	//size_t n = ft::to_num(s);
	std::cout << n;
	return 0;
}
