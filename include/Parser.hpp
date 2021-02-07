#ifndef WEBSERV_PARSER_HPP
#define WEBSERV_PARSER_HPP

# include "ServConfig.hpp"
# include <fstream>

class Parser
{
public:
	Parser(char* file);
	Parser(const Parser &copy);
	~Parser();

	class ParserException: public std::exception {
	public:
		class CannotOpenFile: public std::exception {
			virtual const char* what() const throw();
		};
		//virtual const char* what() const throw();
	};

private:
	Parser();
	Parser &operator=(const Parser &copy);

	//std::vector<ServConfig>servers;
};

#endif //WEBSERV_PARSER_HPP
