#include <sstream>
#include "utils.h"

bool ft::isalldigits(const std::string &str) {
	return (str.find_first_not_of("0123456789") == std::string::npos);
}

void ft::ws_to_tab(std::string &str) {
	for (size_t i = 0; i < str.size(); ++i)
		if (str[i] == ' ')
			str[i] = '\t';
}

void ft::trim(std::string &str, char c) {
	str.erase(0, str.find_first_not_of(c));
	str.erase(str.find_last_not_of(c) + 1);
}

/*void ft::cut_char_from_end(std::string &str, const std::string &charset) {
	std::string::iterator it;
	for (size_t i = 0; i < charset.size(); ++i)
		if (*(it = --str.end()) == charset[i])
			str.erase(it);
}*/

size_t ft::to_num(const std::string &str, bool hex)
{
	size_t n;
	std::stringstream ss(str);

	if (hex)
		ss << std::hex << str;
	else
		ss << str;
	if (!(ss >> n))
		throw std::runtime_error("Invalid conversion to number");
	return (n);
}

std::istream &ft::getline(std::istream &ifs, std::string &line) {
	char ch;
	line.clear();
	while (ifs.get(ch) && ch != '\n')
		line.push_back(ch);
	return ifs;
}

std::vector<std::string> ft::split(const std::string &str, const char delim) {
	size_t start;
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

void ft::tolower(std::string &s) {
	transform(s.begin(), s.end(), s.begin(), ::tolower);
}
