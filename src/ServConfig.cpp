#include "response.h"
#include "ServConfig.h"

ServConfig::ServConfig() :
	_port(0),
	_parsed(0) {}

ServConfig::~ServConfig() {}

ServConfig::ServConfig(const ServConfig &copy) {
	*this = copy;
}

ServConfig &ServConfig::operator=(const ServConfig &copy) {
	this->_names = copy._names;
	this->_host = copy._host;
	this->_port = copy._port;
	this->_root = copy._root;
	this->_locations = copy._locations;
	this->_errorPages = copy._errorPages;
	this->_sockFd = copy._sockFd;
	this->_sockAddr = copy._sockAddr;
	this->_parsed = copy._parsed;
	return (*this);
}

void ServConfig::setNames(const std::vector<std::string> &names) {
	_names = names;
}

void ServConfig::setHost(const std::string &host) {
	if (_parsed & 1)
		throw DuplicateDirective("listen");
	_host = host;
	_parsed |= 1;
}

void ServConfig::setPort(size_t port) {
	if (_parsed & 2)
		throw DuplicateDirective("listen");
	_port = port;
	_parsed |= 2;
}

void ServConfig::setRoot(const std::string &root) {
	if (_parsed & 4)
		throw DuplicateDirective("root");
	_root = root;
	_parsed |= 4;
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
#ifdef BONUS

	std::cmatch match;
	std::string matched_part;
	int locationIndex = -1;
	std::vector<Location>::const_iterator it = _locations.begin();

	for (int res = 0; it != _locations.end(); it++, res++) {
		std::regex regex(it->getPathR());
		if (std::regex_search(reqPath.c_str(), match, regex)) {
			if (match.str(0).size() > matched_part.size()) {
				matched_part = match.str(0);
				locationIndex = res;
			}
		}
	}
	if (locationIndex == -1)
		throw RespException(Response(404));
	const_cast< Location& >(_locations.at(locationIndex)).updatePath(matched_part);
	return _locations.at(locationIndex);

#else

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
	return _locations.at(locationIndex);

#endif
}

void	ServConfig::updateLocationRoot(int index, const std::string &root) {
	this->_locations.at(index).updateRoot(root);
}
