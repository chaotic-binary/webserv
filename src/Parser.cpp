#include "../include/Parser.hpp"

Parser::Parser() {}

Parser::Parser(char* file)
{
	std::string content;
	std::ifstream ifs(file);
	if (!ifs)
		throw ParserException::CannotOpenFile();
	ifs.seekg(0,std::ios::end);
	std::streampos len = ifs.tellg();
	ifs.seekg(0,std::ios::beg);
	content.resize(len);
	ifs.read(&content[0], len);
	std::cout << content << std::endl;//
	ifs.close();
}

Parser::~Parser() {}

Parser::Parser(const Parser &copy) {
	*this = copy;
}

Parser	&Parser::operator=(const Parser &copy)
{
	return (*this);
}

const char*		Parser::ParserException::CannotOpenFile::what() const throw()
{
	return "Cannot open file";
}

