#ifndef WEBSERV_SERVCONFIG_HPP
#define WEBSERV_SERVCONFIG_HPP

# include <iostream>
# include <vector>
# include <map>

class ServConfig
{
public:
	ServConfig();
	~ServConfig();

private:
	ServConfig(const ServConfig &copy);
	ServConfig &operator=(const ServConfig &copy);

	class Location
	{
	public:


	private:
		Location();
		Location(const Location &copy);
		Location &operator=(const Location &copy);

		std::string					_root;
		std::string					_index;
		std::string					_cgiPath;
		std::string					_uploadPath;
		size_t						_maxBody;
		bool						_autoindex;
		bool						_uploadEnable;
		std::vector<std::string>	_cgiExtensions;
		std::vector<std::string>	_methods;
	};

	std::vector<std::string>		_names;
	std::string						_host;
	std::string						_root;
	size_t							_port;
	std::vector<Location>			_locations;
	std::map<int, std::string>		_errorPages;
};

#endif //WEBSERV_SERVCONFIG_HPP
