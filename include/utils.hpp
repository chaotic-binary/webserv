#pragma once

# include <vector>
# include <string>
# include <exception>
# include <fstream>

namespace ft {
	bool isalldigits(const std::string &str);

	void ws_to_tab(std::string &str);

	void trim(std::string &str);

	std::vector<std::string> split(const std::string &str, const char delim);

	std::istream &getline(std::istream &ifs, std::string &line);

	template <typename T>
	std::string to_str(T n) {
		std::string res;
		std::stringstream ss;
		if (!(ss << n) || !(ss >> res)) {
			std::string s = "Conversion impossible";
			throw std::runtime_error(s);
		}
		//TODO: do we need exception?
		return res;
	}
}
