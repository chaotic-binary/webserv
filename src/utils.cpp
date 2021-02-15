#include <sstream>
#include "utils.hpp"

bool ft::isalldigits(const std::string &str) {
	return (str.find_first_not_of("0123456789") == std::string::npos);
}

void ft::ws_to_tab(std::string &str) {
	for (int i = 0; i < str.size(); ++i)
		if (str[i] == ' ')
			str[i] = '\t';
}

void ft::trim(std::string &str) {
	str.erase(0, str.find_first_not_of('\t'));
	str.erase(str.find_last_not_of('\t') + 1);
}

std::istream &ft::getline(std::istream &ifs, std::string &line) {
	char ch;
	line.clear();
	while (ifs.get(ch) && ch != '\n')
		line.push_back(ch);
	return ifs;
}

std::vector<std::string> ft::split(const std::string &str, const char delim) {
	size_t start = 0;
	size_t end = 0;
	std::vector<std::string> res;

	while (end != std::string::npos) {
		start = str.find_first_not_of(delim, end);
		end = str.find_first_of(delim, start);
		if (start != std::string::npos || end != std::string::npos)
			res.push_back(str.substr(start, end - start));
	}
	return res;
}
