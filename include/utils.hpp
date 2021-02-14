#ifndef WEBSERV_UTILS_HPP
#define WEBSERV_UTILS_HPP

# include <vector>
# include <string>
# include <exception>
# include <fstream>

bool						isalldigits(const std::string &str);
void						ws_to_tab(std::string &str);
void						trim_str(std::string &str);
std::vector<std::string>	split_str(const std::string& str, const char delim);
std::istream& ft_getline(std::istream& ifs, std::string& line);

template <typename T>
std::string 				ft_to_str(T n) {
	std::string res;
	std::stringstream ss;
	if (!(ss << n) || !(ss >> res)) {
		std::string s = "Conversion impossible";
		throw std::runtime_error(s);
	}
	//TODO: do we need exception?
	return res;
}

#endif //WEBSERV_UTILS_HPP
