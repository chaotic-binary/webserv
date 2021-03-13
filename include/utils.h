#pragma once

# include <vector>
# include <map>
# include <string>
# include <exception>
# include <sstream>

namespace ft {
	bool isalldigits(const std::string &str);

	void ws_to_tab(std::string &str);

	void trim(std::string &str);

	void cut_char_from_end(std::string &str, const std::string &charset);

	std::vector<std::string> split(const std::string &str, const char delim);

	std::istream &getline(std::istream &ifs, std::string &line);

	template <typename T>
	std::string	to_str(T n) {
		std::string res;
		std::stringstream ss;
		if (!(ss << n) || !(ss >> res)) {
			std::string s = "Conversion impossible";
			throw std::runtime_error(s);
		}
		//TODO: do we need exception?
		return res;
	}

	size_t	to_num(const std::string &str, bool hex = false);

	void	tolower(std::string &s);

} //namespace ft

template <class T, class Alloc>
std::ostream &operator<<(std::ostream &os, const std::vector<T, Alloc> &c) {
	for (typename std::vector<T,Alloc>::const_iterator it = c.begin(); it != c.end(); it++) {
		if (it != c.begin())
			os << '\t';
		os << *it;
	}
	return os;
}

template < class Key, class V, class Compare, class Alloc >
std::ostream &operator<<(std::ostream &os, const std::map<Key, V, Compare, Alloc> &c) {
	typename std::map<Key, V>::const_iterator it;
	for (it = c.begin(); it != c.end(); ++it) {
		//if (it != c.begin())
			os << "\t";
		os << it->first << "=>" << it->second << std::endl;
	}
	return os;
}