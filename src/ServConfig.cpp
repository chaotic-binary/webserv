#include <response.h>
#include "ServConfig.h"

ServConfig::ServConfig() :
	_port(-1) {}

ServConfig::~ServConfig() {}

ServConfig::ServConfig(const ServConfig &copy) :
	_names(copy._names),
	_host(copy._host),
	_port(copy._port),
	_root(copy._root),
	_locations(copy._locations),
	_errorPages(copy._errorPages) {}

ServConfig &ServConfig::operator=(const ServConfig &copy) {
	this->_names = copy._names;
	this->_host = copy._host;
	this->_port = copy._port;
	this->_root = copy._root;
	this->_locations = copy._locations;
	this->_errorPages = copy._errorPages;
	return (*this);
}

void ServConfig::setNames(const std::vector<std::string> &names) {
	_names = names;
}

void ServConfig::setHost(const std::string &host) {
	if (!_host.empty())
		throw std::runtime_error("Double listen instruction");
	_host = host;
}

void ServConfig::setPort(long long port) {
	if (_port != -1)
		throw std::runtime_error("Double or wrong listen instruction");
	_port = port;
}

void ServConfig::setRoot(const std::string &root) {
	if (!_root.empty())
		throw std::runtime_error("Double root instruction");
	_root = root;
	if (_root.back() != '/')
		_root += '/';
}

void ServConfig::setLocations(const std::vector<Location> &locations) {
	_locations = locations;
}

void ServConfig::setErrorPages(const std::map<int, std::string> &errorPages) {
	_errorPages = errorPages;
}

void ServConfig::addErrorPage(const std::pair<int, std::string> &p) {
	_errorPages.insert(p);
}

void ServConfig::addLocation(const Location &loc) {
	_locations.push_back(loc);
}

const std::vector<std::string> &ServConfig::getNames() const {
	return _names;
}

const std::string &ServConfig::getHost() const {
	return _host;
}

long long ServConfig::getPort() const {
	return _port;
}

const std::string &ServConfig::getRoot() const {
	return _root;
}

const std::vector<Location> &ServConfig::getLocations() const {
	return _locations;
}

const std::map<int, std::string> &ServConfig::getErrorPages() const {
	return _errorPages;
}

void ServConfig::setSockFd(int sockFd) {
	_sockFd = sockFd;
}

void ServConfig::setSockAddr(const sockaddr_in &sockAddr) {
	_sockAddr = sockAddr;
}

int ServConfig::getSockFd() const {
	return (this->_sockFd);
}

sockaddr_in ServConfig::getSockAddr() const {
	return (this->_sockAddr);
}

sockaddr_in &ServConfig::getSockAddr() {
	return (this->_sockAddr);
}

const Location &ServConfig::getLocation(const std::string &reqPath) const {
	std::vector<Location>::const_iterator it = _locations.begin();
	size_t maxCoincidence = 0;
	int locationIndex = -1;

	for (int res = 0; it != _locations.end(); it++, res++) {
		if (reqPath.compare(0, it->getPath().size(), it->getPath()) == 0
		&& (reqPath[it->getPath().size()] == '/' ||  reqPath == it->getPath() || it->getPath() == "/"))
		{
			if(it->getPath().size() < maxCoincidence)
				continue;
			locationIndex = res;
			maxCoincidence = it->getPath().size();
		}
	}
	if (locationIndex == -1)
		throw RespException(Response(404));
	return _locations[locationIndex];
}

void	ServConfig::updateLocationRoot(int index, const std::string &root) {
	this->_locations.at(index).setRoot(root);
}
