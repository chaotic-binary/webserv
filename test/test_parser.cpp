#include "Parser.h"
#include <fcntl.h>
#include <Request.h>

int main(int ac, char **av) {
	if (ac != 2) {
		std::cerr << "Wrong number of arguments: one required\n";
		return (1);
	}
	try {

		Parser p;
		p.parse(av[1]);
		std::cout <<  p;//test

	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
/*	int fd = open("../test/request.http", O_RDONLY);
	std::cout << "fd: " << fd << std::endl;
	Request req_(fd);
	try {
		std::cout << "res: " << (req_.receive()) << std::endl;

		std::cout << "<REQUEST\n" << req_ << std::endl;
		std::cout << "REQUEST>\n"; //test
	}
	catch (std::exception &) {
		std::cout << "TODO: handle the half msg!!!" << std::endl;
	}*/
	/*std::string reqTarget = "https://www.google.com/search?newwindow=1&sxsrf=ALeKk01dv4phJBZU2qo_07uEdtAwnyF9DQ%3A1616007720761&ei=KFJSYNDvLYOQwPAPlKaGuAQ&q=cgi+query_string&oq=cgi+%D0%B7%D0%B0%D0%BF%D1%80%D0%BE%D1%81+&gs_lcp=Cgdnd3Mtd2l6EAEYADIGCAAQFhAeMgYIABAWEB4yBggAEBYQHjIGCAAQFhAeMgYIABAWEB4yBggAEBYQHjIGCAAQFhAeMgYIABAWEB4yBggAEBYQHjIGCAAQFhAeOgcIABBHELADUK_uAViv7gFgxfsBaAFwAngAgAE_iAF3kgEBMpgBAKABAaoBB2d3cy13aXrIAQjAAQE&sclient=gws-wiz";
	size_t i;
	std::string queryString;
	std::cout << reqTarget << std::endl << std::endl;
	if ((i = reqTarget.find('?')) != std::string::npos)
	{
		queryString = reqTarget.substr(i + 1, reqTarget.size() -1);
		reqTarget.erase(i, reqTarget.size());
	}
	std::cout << "T= " << reqTarget << std::endl;
	std::cout << "Q= " << queryString << std::endl;*/
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
