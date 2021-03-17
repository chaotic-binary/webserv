#include "../include/Location.h"

Location::Location()
		: _maxBody(DEF_MAX_BODY),
		  _autoindex(false),
		  _uploadEnable(false)
{}

Location::Location(const Location &copy)
{
	*this = copy;
}

Location::~Location()
{}

Location &Location::operator=(const Location &copy)
{
	this->_path = copy._path;
	this->_root = copy._root;
	this->_index = copy._index;
	this->_cgiPath = copy._cgiPath;
	this->_uploadPath = copy._uploadPath;
	this->_maxBody = copy._maxBody;
	this->_autoindex = copy._autoindex;
	this->_uploadEnable = copy._uploadEnable;
	this->_cgiExtensions = copy._cgiExtensions;
	this->_methods = copy._methods;
	return (*this);
}

std::vector<std::string> Location::methodsParser;

void Location::initMethodsParser()
{
	methodsParser.push_back("GET");
	methodsParser.push_back("HEAD");
	methodsParser.push_back("POST");
	methodsParser.push_back("PUT");
	methodsParser.push_back("DELETE");
	methodsParser.push_back("CONNECT");
	methodsParser.push_back("OPTIONS");
	methodsParser.push_back("TRACE");
}

const std::string &Location::getName() const
{
	return _path;
}

const std::string &Location::getRoot() const
{
	return _root;
}

const std::string &Location::getIndex() const
{
	return _index;
}

const std::string &Location::getCgiPath() const
{
	return _cgiPath;
}

const std::string &Location::getUploadPath() const
{
	return _uploadPath;
}

size_t Location::getMaxBody() const
{
	return _maxBody;
}

bool Location::getAutoindex() const
{
	return _autoindex;
}

bool Location::getUploadEnable() const
{
	return _uploadEnable;
}

const std::vector<std::string> &Location::getCgiExtensions() const
{
	return _cgiExtensions;
}

const std::vector<e_methods> &Location::getMethods() const
{
	return _methods;
}

const std::vector<std::string> &Location::getMethodsParser()
{
	return methodsParser;
}

void Location::setName(const std::string &name)
{
	_path = name;
}

void Location::setRoot(const std::string &root)
{
	_root = root;
	if (_root.back() != '/')
		_root += '/';
}

void Location::setIndex(const std::string &index)
{
	_index = index;
}

void Location::setCgiPath(const std::string &cgiPath)
{
	_cgiPath = cgiPath;
}

void Location::setUploadPath(const std::string &uploadPath)
{
	_uploadPath = uploadPath;
}

void Location::setMaxBody(size_t maxBody)
{
	_maxBody = maxBody;
	//TODO: check max min range?
}

void Location::setAutoindex(bool autoindex)
{
	_autoindex = autoindex;
}

void Location::setUploadEnable(bool uploadEnable)
{
	_uploadEnable = uploadEnable;
}

void Location::setCgiExtensions(const std::vector<std::string> &cgiExtensions)
{
	_cgiExtensions = cgiExtensions;
}

void Location::setMethods(const std::vector<e_methods> &methods)
{
	_methods = methods;
}

void Location::setMethodsFromStr(const std::vector<std::string> &methods)
{
	std::vector<std::string>::const_iterator it;
	for (size_t i = 0; i < methods.size(); ++i)
	{
		if ((it = find(methodsParser.begin(), methodsParser.end(), methods[i])) != methodsParser.end())
			_methods.push_back(static_cast<e_methods>(it - methodsParser.begin()));
		else
			throw LocException::WrongMethod();
	}
}

void Location::setAutoindexFromStr(const std::string &str)
{
	setAutoindex(getBoolFromStr(str));
}

void Location::setUploadEnableFromStr(const std::string &str)
{
	setUploadEnable(getBoolFromStr(str));
}

bool Location::getBoolFromStr(const std::string &str)
{
	if (str == "on")
		return (true);
	else if (str == "off")
		return (false);
	else
		throw LocException::WrongOnOff();
}

static std::ostream &operator<<(std::ostream &os, const std::vector<std::string> &v)
{
	for (size_t i = 0; i < v.size(); ++i)
		os << "\t\t" << v[i] << std::endl;
	return os;
}

std::ostream &operator<<(std::ostream &os, const std::vector<e_methods> &v)
{
	for (size_t i = 0; i < v.size(); ++i)
	{
		std::vector<std::string>::const_iterator it;
		std::vector<std::string> methods = Location::getMethodsParser();
		os << "\t\t" << methods.at(v[i]) << std::endl;
	}
	return os;
}

std::ostream &operator<<(std::ostream &os, const Location &location)
{
	os << "\tname: " << location.getName() << std::endl;
	os << "\troot: " << location.getRoot() << std::endl;
	os << "\tindex: " << location.getIndex() << std::endl;
	os << "\tcgi_path: " << location.getCgiPath() << std::endl;
	os << "\tupload_path: " << location.getUploadPath() << std::endl;
	os << "\tmax body: " << location.getMaxBody() << std::endl;
	os << "\tautoindex: " << (location.getAutoindex() ? "on" : "off") << std::endl;
	os << "\tupload enable: " << (location.getUploadEnable() ? "on" : "off") << std::endl;
	const std::vector<std::string> &v = location.getCgiExtensions();
	os << "\tcgi extensions: \n" << v;
	const std::vector<e_methods> &m = location.getMethods();
	os << "\tmethods: \n" << m;
	return os;
}

const std::string &Location::getPath() const
{
	return _path;
}

const char *Location::LocException::WrongMethod::what() const throw()
{
	return "Wrong method";
}

const char *Location::LocException::WrongOnOff::what() const throw()
{
	return "Wrong value: \"on\" or \"off\" required";
}
