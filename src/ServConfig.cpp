#include "ServConfig.hpp"

ServConfig::ServConfig() :
		_port(0) { }

ServConfig::~ServConfig() { }

ServConfig::ServConfig(const ServConfig &copy) :
		_names(copy._names),
		_host(copy._host),
		_port(copy._port),
		_root(copy._root),
		_locations(copy._locations),
		_errorPages(copy._errorPages) { }

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
	_host = host;
	//TODO: validate? turn to num value while parsing?
}

void ServConfig::setPort(size_t port) {
	_port = port;
	//TODO: check max min range?
}

void ServConfig::setRoot(const std::string &root) {
	_root = root;
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

size_t ServConfig::getPort() const {
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

void ServConfig::setSockFd(int sockFd)
{
	_sockFd = sockFd;
}

void ServConfig::setSockAddr(const sockaddr_in &sockAddr)
{
	_sockAddr = sockAddr;
}

int ServConfig::getSockFd() const
{
	return (this->_sockFd);
}

sockaddr_in ServConfig::getSockAddr() const
{
	return (this->_sockAddr);
}
sockaddr_in &ServConfig::getSockAddr()
{
	return (this->_sockAddr);
}
/*Location &ServConfig::getLocation(int i) {
	return _locations[i];
}*/

