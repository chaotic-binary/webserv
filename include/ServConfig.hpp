#pragma once

# include "../include/Location.hpp"

#define LOCALHOST "127.0.0.1"

class ServConfig {
public:
	ServConfig();
	ServConfig(const ServConfig &copy);
	~ServConfig();
	ServConfig &operator=(const ServConfig &copy);

	void setNames(const std::vector<std::string> &names);
	void setHost(const std::string &host);
	void setPort(size_t port);
	void setRoot(const std::string &root);
	void setLocations(const std::vector<Location> &locations);
	void setErrorPages(const std::map<int, std::string> &errorPages);

	void addErrorPage(const std::pair <int, std::string> &);
	void addLocation(const Location &loc);

	const std::vector<std::string> &getNames() const;
	const std::string &getHost() const;
	size_t getPort() const;
	const std::string &getRoot() const;
	const std::vector<Location> &getLocations() const;
	const std::map<int, std::string> &getErrorPages() const;

	Location &getLocation(int i);

private:
	std::vector<std::string>		_names;
	std::string						_host;
	size_t							_port;
	std::string						_root;
	std::vector<Location>			_locations;
	std::map<int, std::string>		_errorPages;
};

