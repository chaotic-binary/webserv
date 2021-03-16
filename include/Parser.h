#pragma once

# include "ServConfig.h"
# include <fstream>
# include <sstream>
# include "utils.h"

class Parser
{
public:
	Parser(char *file);

	Parser(const Parser &copy);

	~Parser();

	const std::vector<ServConfig> &getServs() const;

	friend std::ostream &operator<<(std::ostream &os, const Parser &parser);

	class ParserException : public std::exception
	{
	public:
		class CannotOpenFile : public std::exception
		{
			virtual const char *what() const throw();
		};

		class UnknownParam : public std::logic_error
		{
		private:
			UnknownParam();

		public:
			UnknownParam(int line) : std::logic_error("Unknown parameter: line:" + ft::to_str(line))
			{};
		};

		class InvalidData : public std::logic_error
		{
		private:
			InvalidData();

		public:
			InvalidData(int line) : std::logic_error("Invalid data format: line:" + ft::to_str(line))
			{};
		};

		class BraceExpected : public std::logic_error
		{
		private:
			BraceExpected();

		public:
			BraceExpected(int line) : std::logic_error("Brace expected: line:" + ft::to_str(line))
			{};
		};
		//virtual const char* what() const throw();
	};

private:
	Parser();

	Parser &operator=(const Parser &copy);

	std::vector<ServConfig> _servs;

	void parse_line(std::string &line, int &brace, ServConfig &main);

	void addMainAndDefaults(const ServConfig &main);

	static void line_to_serv(std::vector<std::string> &, ServConfig &);

	static void line_to_loc(std::vector<std::string> &, Location &);

	static void parseListen(const std::vector<std::string> &, ServConfig &);

	static void parseHost(const std::string &, ServConfig &);

	static void parseErrorPages(const std::vector<std::string> &, ServConfig &);

	static void parseServNames(const std::vector<std::string> &, ServConfig &);

	static void parseServRoot(const std::vector<std::string> &, ServConfig &);

	static void parseLocName(const std::vector<std::string> &, Location &);

	static void parseMaxBody(std::string &, Location &);

	static std::map<std::string, void (*)(const std::vector<std::string> &, ServConfig &)> servParser;
	static std::map<std::string, void (Location::*)(const std::string &)> locParser;
	static std::map<std::string, void (Location::*)(const std::vector<std::string> &)> locArrParser;
	static int line_num;

	static void initServParser();

	static void initLocParser();

	static void initLocArrParser();

	static void trim_semicolon(std::string &);

	static size_t to_num(const std::string &str);
};

