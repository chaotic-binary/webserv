#include "Location.h"

Location::Location()
	: _maxBody(DEF_MAX_BODY),
	  _autoindex(false),
	  _uploadEnable(false),
	  _parsed(0) {}

Location::Location(const Location &copy) {
	*this = copy;
}

Location::~Location() {}

std::vector<std::string> Location::methodsParser;

void Location::initMethodsParser() {
	methodsParser.push_back("GET");
	methodsParser.push_back("HEAD");
	methodsParser.push_back("POST");
	methodsParser.push_back("PUT");
	methodsParser.push_back("DELETE");
	methodsParser.push_back("CONNECT");
	methodsParser.push_back("OPTIONS");
	methodsParser.push_back("TRACE");
}

const std::string &Location::getName() const {
	return _path;
}

const std::string &Location::getRoot() const {
	return _root;
}

const std::string &Location::getIndex() const {
	return _index;
}

const std::string &Location::getCgiIndex() const {
	return _cgi_index;
}

const std::string &Location::getCgiPath() const {
	return _cgiPath;
}

const std::string &Location::getUploadPath() const {
	return _uploadPath;
}

size_t Location::getMaxBody() const {
	return _maxBody;
}

bool Location::getAutoindex() const {
	return _autoindex;
}

bool Location::getUploadEnable() const {
	return _uploadEnable;
}

const std::vector<std::string> &Location::getCgiExtensions() const {
	return _cgiExtensions;
}

const std::vector<e_methods> &Location::getMethods() const {
	return _methods;
}

const std::vector<std::string> &Location::getMethodsParser() {
	return methodsParser;
}

void Location::setName(const std::string &name) {
	_path = name;
}

void Location::setRoot(const std::string &root) {
	if (_parsed & 1)
		throw DuplicateInstruction("root");
	_root = root;
	_parsed |= 1;
	if (_root.back() != '/')
		_root += '/';
}

void Location::setIndex(const std::string &index) {
	if (_parsed & 2)
		throw DuplicateInstruction("index");
	_index = index;
	_parsed |= 2;
}

void Location::setCgiIndex(const std::string &index) {
	if (_parsed & 4)
		throw DuplicateInstruction("cgi index");
	_cgi_index = index;
	_parsed |= 4;
}
void Location::setCgiPath(const std::string &cgiPath) {
	if (_parsed & 8)
		throw DuplicateInstruction("cgi path");
	_cgiPath = cgiPath;
	_parsed |= 8;
}

void Location::setUploadPath(const std::string &uploadPath) {
	if (_parsed & 16)
		throw DuplicateInstruction("upload path");
	_uploadPath = uploadPath;
	_parsed |= 16;
}

void Location::setMaxBody(size_t maxBody) {
	if (_parsed & 32)
		throw DuplicateInstruction("client max body size");
	_maxBody = maxBody;
	_parsed |= 32;
}

void Location::setAutoindex(bool autoindex) {
	if (_parsed & 64)
		throw DuplicateInstruction("autoindex");
	_autoindex = autoindex;
	_parsed |= 64;
}

void Location::setUploadEnable(bool uploadEnable) {
	if (_parsed & 128)
		throw DuplicateInstruction("upload enable");
	_uploadEnable = uploadEnable;
	_parsed |= 128;
}

void Location::setCgiExtensions(const std::vector<std::string> &cgiExtensions) {
	_cgiExtensions = cgiExtensions;
	for (size_t i = 0; i < _cgiExtensions.size(); ++i) {
		if (_cgiExtensions[i].front() != '.')
			_cgiExtensions[i] = "." + _cgiExtensions[i];
	}
}

void Location::setMethods(const std::vector<e_methods> &methods) {
	_methods = methods;
}

void Location::setMethodsFromStr(const std::vector<std::string> &methods) {
	std::vector<std::string>::const_iterator it;
	for (size_t i = 0; i < methods.size(); ++i) {
		if ((it = find(methodsParser.begin(), methodsParser.end(), methods[i])) != methodsParser.end())
			_methods.push_back(static_cast<e_methods>(it - methodsParser.begin()));
		else
			throw std::runtime_error("Wrong method");
	}
}

void Location::setAutoindexFromStr(const std::string &str) {
	setAutoindex(getBoolFromStr(str));
}

void Location::setUploadEnableFromStr(const std::string &str) {
	setUploadEnable(getBoolFromStr(str));
}

bool Location::getBoolFromStr(const std::string &str) {
	if (str == "on")
		return (true);
	else if (str == "off")
		return (false);
	else
		throw std::runtime_error("Wrong value: \"on\" or \"off\" required");
}

static std::ostream &operator<<(std::ostream &os, const std::vector<std::string> &v) {
	for (size_t i = 0; i < v.size(); ++i)
		os << "\t\t" << v[i] << std::endl;
	return os;
}

const std::string &Location::getPath() const {
	return _path;
}

std::ostream &operator<<(std::ostream &os, const std::vector<e_methods> &v) {
	for (size_t i = 0; i < v.size(); ++i) {
		std::vector<std::string>::const_iterator it;
		std::vector<std::string> methods = Location::getMethodsParser();
		os << "\t\t" << methods.at(v[i]) << std::endl;
	}
	return os;
}

std::ostream &operator<<(std::ostream &os, const Location &location) {
	os << "\tname: " << location.getName() << std::endl;
	os << "\troot: " << location.getRoot() << std::endl;
	os << "\tindex: " << location.getIndex() << std::endl;
	os << "\tcgi_path: " << location.getCgiPath() << std::endl;
	os << "\tcgi_index: " << location.getCgiIndex() << std::endl;
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