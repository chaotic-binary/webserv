#include "../include/Parser.hpp"

bool	isalldigits(const std::string &str) {
	return (str.find_first_not_of("0123456789") == std::string::npos);
}

size_t to_num(const std::string &str) {
	if (!isalldigits(str))
		throw Parser::ParserException::InvalidData();

	size_t n;
	std::stringstream ss(str);
	if (!(ss >> n))
		throw Parser::ParserException::InvalidData();
	return (n);
}

void	ws_to_tab(std::string &str)
{
	for (int i = 0; i < str.size(); ++i)
		if (str[i] == ' ')
			str[i] = '\t';
}

void	trim_str(std::string &str)
{
	str.erase(0, str.find_first_not_of('\t'));
	str.erase(str.find_last_not_of('\t') + 1);
}

void	check_semicolon(std::string &str)
{
	//if (str[str.size() - 1] != ';')
	//	throw
	str.erase(str.find_last_not_of(';') + 1);
	str.erase(str.find_last_not_of('\t') + 1);
}

std::vector<std::string>	split_str(const std::string& str, const std::string& delim) {
	size_t start = 0;
	size_t end = 0;
	std::vector<std::string> res;

	while (end != std::string::npos) {
		start = str.find_first_not_of(delim, end);
		end = str.find_first_of(delim, start);
		if (end != std::string::npos || start != std::string::npos)
			res.push_back(str.substr(start, end - start));
	}
	return res;
}

void	Parser::line_to_serv(std::vector<std::string> &v, ServConfig &serv)
{
	std::map<std::string, void (Parser::*) (const std::vector<std::string> &, ServConfig &)> parser;
	parser["listen"] = &Parser::parseListen;
	parser["server_name"] = &Parser::parseServNames;
	parser["root"] = &Parser::parseServRoot;
	parser["error_page"] = &Parser::parseErrorPages;

	std::map<std::string, void (Parser::*) (const std::vector<std::string> &, ServConfig &)>::iterator it;
	if ((it = parser.find(v[0])) != parser.end()) {
		//std::cout << "find func" << std::endl;//
		(this->*it->second)(v, serv);
	}
	else
		throw ParserException::UnknownParam();
}

void	Parser::line_to_loc(std::vector<std::string> &v, Location &loc)
{
	std::map<std::string, void (Location::*) (const std::string &)> parser;
	parser["root"] = &Location::setRoot;
	parser["upload_path"] = &Location::setUploadPath;
	parser["cgi_path"] = &Location::setCgiPath;
	parser["index"] = &Location::setIndex;

	std::map<std::string, void (Location::*) (const std::vector<std::string> &)> arr_parser;
	arr_parser["cgi_extension"] = &Location::setCgiExtensions;
	arr_parser["method"] = &Location::setMethods;

	std::map<std::string, void (Location::*) (const std::vector<std::string> &)>::iterator ita;
	if ((ita = arr_parser.find(v[0])) != arr_parser.end()) {
		//std::cout << "find loc arr func" << std::endl;//
		if (v.size() < 2)
			throw ParserException::InvalidData();
		v.erase(v.begin());
		(loc.*ita->second)(v);
	}
	else {
		if (v.size() != 2)
			throw ParserException::InvalidData();
		std::map<std::string, void (Location::*) (const std::string &)>::iterator it;
		if ((it = parser.find(v[0])) != parser.end()) {
			//std::cout << "find loc func" << std::endl;//
			(loc.*it->second)(v[1]);
		} else if (v[0] == "client_max_body_size")
			parseMaxBody(v[1], loc);
		else if (v[0] == "autoindex")
			loc.setAutoindex(parseBool(v[1]));
		else if (v[0] == "upload_enable")
			loc.setUploadEnable(parseBool(v[1]));
		else
			throw ParserException::UnknownParam();
	}
}

void	Parser::parse_line(std::string &line, int &brace, ServConfig &main)
{
	//static bool main_context = false;
	static int loc_context = 0;
	static int serv = -1;
	static int loc = -1;
	//static std::vector<Location> locations;
	static Location location;

	if (line == "server\t{" || line == "server{") {
		if (brace)
			throw ParserException::BraceExpected();
		++serv;
		ServConfig s = ServConfig();
		_servs.push_back(s);
		++brace;
		loc = -1;
	} else if (line.substr(0, line.find("\t")) == "location") {
		if (!brace && serv != -1)
			throw ParserException::InvalidData();
		if (!loc_context)
			++loc;
		++brace;
		++loc_context;
		//location = Location();
		//Location l;
		parseLocName(split_str(line, "\t"), location);
		//(serv < 0) ? main.addLocation(location) : _servs[serv].addLocation(location);

	} else {
		if (line[0] == '}' && line.size() == 1) {
			if (!brace)
				throw ParserException::BraceExpected();
			if (loc_context) {
				//locations.push_back(location);
				(serv < 0) ? main.addLocation(location) : _servs[serv].addLocation(location);
				//std::cout<<location<<std::endl;
				location = Location();
				--loc_context;
			} //else //{
				//for (int i = 0; i < locations.size(); ++i)
				//	std::cout << "loc name" << locations[i].getName() << std::endl;
				//locations.clear();
			//}
			--brace;
		}
		else {
			check_semicolon(line);
			std::vector<std::string> v = split_str(line, "\t");
			//if (v.empty())//		return ;
			if (v[1].empty()) {
				throw ParserException::InvalidData();
				return ;
			}
			if (loc_context) { //std::cout << "loc " << loc << std::endl;
				line_to_loc(v, location);
			}
			else if (brace == 1)
				line_to_serv(v, _servs[serv]);
			else if (!brace && serv < 0) {
				//if (!main_context) {
				//	main_context = true;
					//main.setPort(0);
					//main.setHost("");}
				line_to_serv(v,  main);
			}
			else
				throw ParserException::InvalidData();
		}
	}
}

Parser::Parser(char* file)
{
	std::ifstream ifs(file);
	if (!ifs)
		throw ParserException::CannotOpenFile();

	std::string line;
	int brace = 0;
	ServConfig main;
	while (std::getline(ifs, line)) {
		//std::cout << line << std::endl << std::endl;
		ws_to_tab(line);
		trim_str(line);
		if (!line.empty() && line[0] != '#')
			parse_line(line, brace, main);
	}
	ifs.close();
	if (brace)
		throw ParserException::BraceExpected();
	addMainAndDefaults(main);
}

Parser::Parser() {}

Parser::~Parser() {}

Parser::Parser(const Parser &copy) {
	*this = copy;
}

Parser	&Parser::operator=(const Parser &copy)
{
	if (this != &copy)
		*this = copy;
	return (*this);
}

void	Parser::parseListen(const std::vector<std::string> &args, ServConfig &serv)
{
	for (int i = 1; i < args.size(); ++i) { 	//std::cout << args[i] << "|" << std::endl;//
		if (args[i].find(':') != args[i].npos) {
			std::vector<std::string> w = split_str(args[1], ":");
			//std::cout << "w0=" << w[0] << "|" << std::endl;//
			//std::cout << "w1=" << w[1] << "|" << std::endl;//
			serv.setHost(w[0]);
			serv.setPort(to_num(w[1]));
		}
		else if (args[i].find('.') != args[i].npos)
			serv.setHost(args[i]);
		else if (args[i] != "localhost")
			serv.setPort(to_num(args[i]));
		//std::cout << "host:" <<  serv.getHost() << " port:" << serv.getPort() << std::endl;
	}
}

void	Parser::parseServNames(const std::vector<std::string> & args, ServConfig & serv)
{
	std::vector<std::string>args_copy = args;
	args_copy.erase(args_copy.begin());
	serv.setNames(args_copy);
	/*std::vector<std::string> names = serv.getNames();
	for (int i = 0; i < names.size(); ++i) {

		std::cout << names[i]  << "!" << std::endl;//
	}*/
}

void	Parser::parseServRoot(const std::vector<std::string> & args, ServConfig & serv)
{
	if (args.size() != 2)
		throw ParserException::InvalidData();
	serv.setRoot(args[1]);
}

void	Parser::parseErrorPages(const std::vector<std::string> & args, ServConfig & serv)
{
	std::pair<int, std::string> p;
	size_t size = args.size();

	if (size < 3)
		throw ParserException::InvalidData();
	for (int i = 1; i < size - 1; ++i) {
		p.first = to_num(args[i]);
		p.second = args[size - 1];
		serv.addErrorPage(p);
	}

	/*std::map<int, std::string> mymap = serv.getErrorPages();
	std::map<int, std::string>::iterator it;
	std::cout << "mymap contains:\n";
	for (it=mymap.begin(); it!=mymap.end(); ++it)
		std::cout << it->first << " => " << it->second << '\n';*/
}

void	Parser::parseLocName(const std::vector<std::string> &args, Location &loc)
{
	size_t  size = args.size();

	if (size != 3)
		throw ParserException::InvalidData();
	if (args[size - 1] != "{")
		throw ParserException::BraceExpected();
	loc.setName(args[1]);
}

bool Parser::parseBool(const std::string &val)
{
	if (val == "on")
		return (true);
	else if (val == "off")
		return (false);
	else
		throw ParserException::InvalidData();
}

void Parser::parseMaxBody(std::string &val, Location &loc)
{
	size_t n = 1;
	char letter = val[val.size() - 1];
	if (!std::isdigit(letter)) {
		letter = toupper(letter);
		switch (letter) {
			case 'K':
				n = 1024;
				break;
			case 'M' :
				n = 1048576;
				break;
			case 'G' :
				n = 1073741824;
				break;
			default:
				throw ParserException::InvalidData();
				break;
		}
		val = val.substr(0, val.find_first_not_of("0123456789"));
	}
	//n *= to_num(val);
	loc.setMaxBody(n * to_num(val));

	//std::cout << "maxbody = " << loc.getMaxBody() << std::endl; //
}

void Parser::addMainAndDefaults(const ServConfig &main)
{
	for (int i = 0; i < _servs.size(); ++i) {
		if (_servs[i].getRoot().empty())
			_servs[i].setRoot(main.getRoot());
		if (_servs[i].getErrorPages().empty())
			_servs[i].setErrorPages(main.getErrorPages());
		if (!main.getLocations().empty()) {
			std::vector<Location> locs = main.getLocations();
			for (int l = 0; l < locs.size(); ++l)
				_servs[i].addLocation(locs[l]);
		}
		if (!_servs[i].getPort())
			main.getPort() ? _servs[i].setPort(main.getPort()) : _servs[i].setPort(80);
		if (_servs[i].getHost().empty())
			main.getHost().empty() ? _servs[i].setHost(LOCALHOST) : _servs[i].setHost(main.getHost());
	}
}

const std::vector<ServConfig> &Parser::getServs() const
{
	return _servs;
}

const char*	Parser::ParserException::CannotOpenFile::what() const throw()
{
	return "Cannot open file";
}

const char	*Parser::ParserException::UnknownParam::what() const throw()
{
	return "Unknown parameter";
}

const char *Parser::ParserException::InvalidData::what() const throw()
{
	return "Invalid data format";
}

const char *Parser::ParserException::BraceExpected::what() const throw()
{
	return "Brace expected";
}

std::ostream &operator<<(std::ostream &os, const Parser &parser)
{
	std::vector<ServConfig> servs = parser.getServs();
	for (int i = 0; i < servs.size(); ++i) {
		os << "SERV[" << i << "]:\n";
		os << "host: " << servs[i].getHost() << std::endl;
		os << "port: " << servs[i].getPort() << std::endl;
		os << "root: " << servs[i].getRoot() << std::endl;

		os << "error pages:\n";
		std::map<int, std::string> mymap = servs[i].getErrorPages();
		std::map<int, std::string>::iterator it;
		for (it=mymap.begin(); it!=mymap.end(); ++it)
			std::cout << it->first << " => " << it->second << '\n';

		std::vector<Location> locations = servs[i].getLocations();
		for (int l = 0; l < locations.size(); ++l) {
			os << "location " << l << std::endl << locations[l] << std::endl;
		}
	}
	return os;
}

