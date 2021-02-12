#include "../include/utils.hpp"

bool	isalldigits(const std::string &str) {
	return (str.find_first_not_of("0123456789") == std::string::npos);
}

void	ws_to_tab(std::string &str)
{
	for (int i = 0; i < str.size(); ++i)
		if (str[i] == ' ')
			str[i] = '\t';
}

void	trim_str(std::string &str)
{
	str.erase(0, str.find_first_not_of('\t'));
	str.erase(str.find_last_not_of('\t') + 1);
}

std::vector<std::string>	split_str(const std::string& str, const char delim) {
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