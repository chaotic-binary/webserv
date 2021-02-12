#ifndef WEBSERV_PARSER_HPP
#define WEBSERV_PARSER_HPP

# include "ServConfig.hpp"
# include <fstream>
# include <sstream>
# include <string>
# include <ostream>

class Parser
{
public:
	Parser(char* file);
	Parser(const Parser &copy);
	~Parser();

	const std::vector<ServConfig> &getServs() const;
	friend std::ostream &operator<<(std::ostream &os, const Parser &parser);

	class ParserException: public std::exception {
	public:
		class CannotOpenFile: public std::exception {
			virtual const char* what() const throw();
		};
		class UnknownParam: public std::exception {
			virtual const char* what() const throw();
		};
		class InvalidData: public std::exception {
			virtual const char* what() const throw();
		};
		class BraceExpected: public std::exception {
			virtual const char* what() const throw();
		};
		//virtual const char* what() const throw();
	};

private:
	Parser();
	Parser &operator=(const Parser &copy);

	std::vector<ServConfig> _servs;

	void parse_line(std::string &line, int &brace, ServConfig &main);
	void line_to_serv(std::vector<std::string> &v, ServConfig &serv);
	void line_to_loc(std::vector<std::string> &v, Location &loc);

	static void parseListen(const std::vector<std::string> &, ServConfig &);
	static void parseErrorPages(const std::vector<std::string> &, ServConfig &);
	static void parseServNames(const std::vector<std::string> &, ServConfig &);
	static void parseServRoot(const std::vector<std::string> &, ServConfig &);
	static void parseLocName(const std::vector<std::string> & args, Location & loc);
	static void parseMaxBody(std::string &val, Location & loc);

	void addMainAndDefaults(const ServConfig &main);

	//static std::map<std::string, void (Parser::*) (const std::vector<std::string> &, ServConfig &)> servParser;
};

//std::ostream &operator<<(std::ostream &os, const Parser &parser);

#endif //WEBSERV_PARSER_HPP
