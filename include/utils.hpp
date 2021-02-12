#ifndef WEBSERV_UTILS_HPP
#define WEBSERV_UTILS_HPP

# include <vector>
# include <string>

bool						isalldigits(const std::string &str);
void						ws_to_tab(std::string &str);
void						trim_str(std::string &str);
std::vector<std::string>	split_str(const std::string& str, const char delim);

#endif //WEBSERV_UTILS_HPP
