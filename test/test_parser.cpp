#include "Parser.h"
#include <fcntl.h>
#include <Request.h>

int main(int ac, char **av) {
	if (ac != 2) {
		std::cerr << "Wrong number of arguments: one required\n";
		return (1);
	}
	try {
		Parser p = Parser(av[1]);

	//	std::cout << p;//test

	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	int fd = open("../test/request_chunked.http", O_RDONLY);
	std::cout << "fd: " << fd << std::endl;
	Request req_(fd);
	try {
		std::cout << "res: " << (req_.receive()) << std::endl;

		std::cout << "<REQUEST\n" << req_ << std::endl;
		std::cout << "REQUEST>\n"; //test
	}
	catch (std::exception &) {
		std::cout << "TODO: handle the half msg!!!" << std::endl;
	}
	/*dup2(open("../test/request_p2.http", O_RDONLY), fd);
	try {
		std::cout << "res: " << (req_.receive()) << std::endl;

		std::cout << "<REQUEST\n" << req_ << std::endl;
		std::cout << "REQUEST>\n"; //test
	}
	catch (std::exception &) {
		std::cout << "TODO: handle the half msg!!!" << std::endl;
	}*/
	return 0;
}
