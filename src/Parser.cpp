#include <cmath>
#include "Parser.h"

std::map<std::string, void (*)(const std::vector<std::string> &, ServConfig &)> Parser::servParser;
std::map<std::string, void (Location::*)(const std::string &)> Parser::locParser;
std::map<std::string, void (Location::*)(const std::vector<std::string> &)> Parser::locArrParser;
int Parser::line_num;

void Parser::initServParser() {
	servParser["listen"] = &Parser::parseListen;
	servParser["server_name"] = &Parser::parseServNames;
	servParser["root"] = &Parser::parseServRoot;
	servParser["error_page"] = &Parser::parseErrorPages;
}

void Parser::initLocParser() {
	locParser["root"] = &Location::setRoot;
	locParser["upload_path"] = &Location::setUploadPath;
	locParser["cgi_path"] = &Location::setCgiPath;
	locParser["index"] = &Location::setIndex;
	locParser["cgi_index"] = &Location::setCgiIndex;
	locParser["autoindex"] = &Location::setAutoindexFromStr;
	locParser["upload_enable"] = &Location::setUploadEnableFromStr;
}

void Parser::initLocArrParser() {
	locArrParser["cgi_extension"] = &Location::setCgiExtensions;
	locArrParser["method"] = &Location::setMethodsFromStr;
}

void Parser::line_to_serv(std::vector<std::string> &v, ServConfig &serv) {
	std::map<std::string, void (*)(const std::vector<std::string> &, ServConfig &)>::const_iterator it;
	if ((it = servParser.find(v[0])) != servParser.end()) {
		try {
			(*it->second)(v, serv);
		} catch (std::exception &e) {
			std::cerr << e.what();
			throw std::runtime_error(" (line " + ft::to_str(line_num) + ")");
		}
	} else
		throw ParserException::UnknownParam(line_num);
}

void Parser::line_to_loc(std::vector<std::string> &v, Location &loc) {
	std::map<std::string, void (Location::*)(const std::vector<std::string> &)>::const_iterator ita;
	if ((ita = locArrParser.find(v[0])) != locArrParser.end()) {
		if (v.size() < 2)
			throw ParserException::InvalidData(line_num);
		v.erase(v.begin());
		try {
			(loc.*ita->second)(v);
		} catch (std::exception &e) {
			std::cerr << e.what();
			throw std::runtime_error(" (line " + ft::to_str(line_num) + ")");
		}
	} else {
		if (v.size() != 2)
			throw ParserException::InvalidData(line_num);
		std::map<std::string, void (Location::*)(const std::string &)>::const_iterator it;
		if ((it = locParser.find(v[0])) != locParser.end()) {
			try {
				(loc.*it->second)(v[1]);
			} catch (std::exception &e) {
				std::cerr << e.what();
				throw std::runtime_error(" (line " + ft::to_str(line_num) + ")");
			}
		} else if (v[0] == "client_max_body_size")
			parseMaxBody(v[1], loc);
		else
			throw ParserException::UnknownParam(line_num);
	}
}

void Parser::parse_line(std::string &line, int &brace, ServConfig &main) {
	static int loc_context = 0;
	static int serv = -1;
	static Location location;

	if (line == "server{" || (line.substr(0, line.find("\t")) == "server")) {
		if (line != "server{") {
			if (line.back() != '{' || (brace))
				throw ParserException::BraceExpected(line_num);
			std::vector<std::string> v = ft::split(line, '\t');
			if (v.size() != 2)
				throw ParserException::InvalidData(line_num);
		}
		++serv;
		ServConfig s = ServConfig();
		_servs.push_back(s);
		++brace;
	} else if (line.substr(0, line.find("\t")) == "location") {
		if ((serv > -1) ? brace != 1 : brace != 0)
			throw ParserException::InvalidData(line_num);
		++brace;
		++loc_context;
		parseLocName(ft::split(line, '\t'), location);

	} else {
		if (line[0] == '}' && line.size() == 1) {
			if (!brace)
				throw ParserException::BraceExpected(line_num);
			if (loc_context) {
				(serv < 0) ? main.addLocation(location) : _servs[serv].addLocation(location);
				location = Location();
				--loc_context;
			}
			--brace;
		} else {
			trim_semicolon(line);
			std::vector<std::string> v = ft::split(line, '\t');
			if (v.size() < 2) {
				throw ParserException::InvalidData(line_num);
			}
			if (loc_context) {
				line_to_loc(v, location);
			} else if (brace == 1)
				line_to_serv(v, _servs[serv]);
			else if (!brace && serv < 0) {
				line_to_serv(v, main);
			} else
				throw ParserException::InvalidData(line_num);
		}
	}
}

const std::vector<ServConfig> &Parser::parse(char *file) {
	std::ifstream ifs(file);
	if (!ifs)
		throw std::runtime_error("Cannot open configuration file");
	initServParser();
	initLocParser();
	initLocArrParser();
	Location::initMethodsParser();
	std::string line;
	int brace = 0;
	line_num = 0;
	ServConfig main;
	while (ft::getline(ifs, line) || !line.empty()) {
		++line_num;
		ft::ws_to_tab(line);
		ft::trim(line, '\t');
		if (!line.empty() && line[0] != '#')
			parse_line(line, brace, main);
	}
	ifs.close();
	if (brace)
		throw ParserException::BraceExpected(line_num);
	if (_servs.empty())
		throw std::runtime_error("Empty configuration file");
	addMainAndDefaults(main);
	return _servs;
}

Parser::Parser() {}

Parser::~Parser() {}

Parser::Parser(const Parser &copy) {
	*this = copy;
}

Parser &Parser::operator=(const Parser &copy) {
	if (this != &copy)
		*this = copy;
	return (*this);
}

void Parser::parseListen(const std::vector<std::string> &args, ServConfig &serv) {
	if (args[1].find(':') != args[1].npos && args.size() == 2) {
		std::vector<std::string> v = ft::split(args[1], ':');
		if (v.size() != 2)
			throw ParserException::InvalidData(line_num);
		parseHost(v[0], serv);
		serv.setPort(to_num(v[1]));
	} else {
		if (args.size() > 3)
			throw ParserException::InvalidData(line_num);
		for (size_t i = 1; i < args.size(); ++i) {
			if (args[i].find('.') != args[i].npos || (args[i] == "localhost")) {
				parseHost(args[i], serv);
			} else {
				serv.setPort(to_num(args[i]));
			}
		}
	}
}

void Parser::parseHost(const std::string &s, ServConfig &serv) {
	if (s == "localhost")
		serv.setHost(LOCALHOST);
	else if (s.find(':') != std::string::npos)
		throw ParserException::InvalidData(line_num);
	else
		serv.setHost(s);
}

void Parser::parseServNames(const std::vector<std::string> &args, ServConfig &serv) {
	std::vector<std::string> names;

	for (size_t i = 1; i < args.size(); ++i) {
		if (args[i] == "localhost")
			names.push_back(LOCALHOST);
		else
			names.push_back(args[i]);
	}
	serv.setNames(names);
}

void Parser::parseServRoot(const std::vector<std::string> &args, ServConfig &serv) {
	if (args.size() != 2)
		throw ParserException::InvalidData(line_num);
	serv.setRoot(args[1]);
}

void Parser::parseErrorPages(const std::vector<std::string> &args, ServConfig &serv) {
	std::pair<int, std::string> p;
	size_t size = args.size();

	if (size < 3)
		throw ParserException::InvalidData(line_num);
	for (size_t i = 1; i < size - 1; ++i) {
		p.first = to_num(args[i]);
		p.second = args.back();
		serv.addErrorPage(p);
	}
}

void Parser::parseLocName(const std::vector<std::string> &args, Location &loc) {
	size_t size = args.size();

	if (args.back() != "{")
		throw ParserException::BraceExpected(line_num);
	if (size != 3)
		throw ParserException::InvalidData(line_num);
	loc.setName(args[1]);
}

void Parser::parseMaxBody(std::string &val, Location &loc) {
	static const size_t KB = pow(2, 10);
	static const size_t MB = pow(2, 20);
	static const size_t GB = pow(2, 30);
	size_t n = 1;
	char letter = val.back();

	if (!std::isdigit(letter)) {
		letter = toupper(letter);
		switch (letter) {
			case 'K': n = KB;
				break;
			case 'M' : n = MB;
				break;
			case 'G' : n = GB;
				break;
			default: throw ParserException::InvalidData(line_num);
				break;
		}
		val = val.substr(0, val.find_first_not_of("0123456789"));
	}
	size_t val_num = to_num(val);
	if (std::numeric_limits<size_t>::max() /  n < val_num)
		throw std::runtime_error("line: " + ft::to_str(line_num) + ": value too big");
	loc.setMaxBody(n * val_num);
}

void Parser::addMainAndDefaults(const ServConfig &main) {
	std::vector<Location> locs;

	for (size_t i = 0; i < _servs.size(); ++i) {
		if (_servs[i].getErrorPages().empty())
			_servs[i].setErrorPages(main.getErrorPages());
		if (!main.getLocations().empty()) {
			locs = main.getLocations();
			for (size_t l = 0; l < locs.size(); ++l)
				_servs[i].addLocation(locs[l]);
		}
		locs = _servs[i].getLocations();
		for (size_t l = 0; l < locs.size(); ++l) {
			if (!_servs[i].getRoot().empty())
				_servs[i].updateLocationRoot(l, _servs[i].getRoot());
			if (!main.getRoot().empty())
				_servs[i].updateLocationRoot(l, main.getRoot());
			if (_servs[i].getLocations()[l].getRoot().empty())
				throw std::runtime_error("No root information");
		}
		if (!_servs[i].getPort())
			main.getPort() ? _servs[i].setPort(main.getPort()) : _servs[i].setPort(80);
		if (_servs[i].getHost().empty())
			main.getHost().empty() ? _servs[i].setHost(LOCALHOST) : _servs[i].setHost(main.getHost());
	}
}

void Parser::trim_semicolon(std::string &str) {
	if (str.find('#') != std::string::npos)
		str.erase(str.find_last_of('#'));
	str.erase(str.find_last_not_of('\t') + 1);
	str.erase(str.find_last_not_of(';') + 1);
	str.erase(str.find_last_not_of('\t') + 1);
}

size_t Parser::to_num(const std::string &str) {
	if (!ft::isalldigits(str))
		throw Parser::ParserException::InvalidData(line_num);
	try {
		return (ft::to_num(str));
	} catch (std::exception &e) {
		throw Parser::ParserException::InvalidData(line_num);
	}
}

const std::vector<ServConfig> &Parser::getServs() const {
	return _servs;
}

std::ostream &operator<<(std::ostream &os, const Parser &parser) {
	std::vector<ServConfig> servs = parser.getServs();
	for (size_t i = 0; i < servs.size(); ++i) {
		os << "SERV[" << i << "]:\n";
		os << "host: " << servs[i].getHost() << std::endl;
		os << "port: " << servs[i].getPort() << std::endl;
		os << "root: " << servs[i].getRoot() << std::endl;
		os << "names: ";
		std::vector<std::string> names = servs[i].getNames();
		for (size_t k = 0; k < names.size(); ++k)
			os << names[k] << " ";
		os << std::endl;
		os << "error pages:\n";
		std::map<int, std::string> mymap = servs[i].getErrorPages();
		std::map<int, std::string>::const_iterator it;
		for (it = mymap.begin(); it != mymap.end(); ++it)
			std::cout << it->first << " => " << it->second << '\n';

		std::vector<Location> locations = servs[i].getLocations();
		for (size_t l = 0; l < locations.size(); ++l) {
			os << "location " << l << std::endl << locations[l] << std::endl;
		}
	}
	return os;
}
