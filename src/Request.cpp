#include <libc.h>
#include "Request.h"
/*
std::vector<std::string>	Request::headersList;

void	Request::headersListInit() {
	headersList.push_back("accept-charsets:");
	headersList.push_back("accept-language:");
	headersList.push_back("authorization:");
	headersList.push_back("content-language:");
	headersList.push_back("content-length:");
	headersList.push_back("content-type:");
	headersList.push_back("date:");
	headersList.push_back("host:");
	headersList.push_back("referer:");
	headersList.push_back("transfer-encoding:");
	headersList.push_back("user-agent:");*/

/*	headersList.push_back("server:");
	headersList.push_back("www-authenticate:");
	headersList.push_back("allow:");
	headersList.push_back("content-location:");
	headersList.push_back("retry-after:");
	headersList.push_back("location:");
	headersList.push_back("last-modified:");*/
//};

Request::Request(const int fd)
		: method(OTHER),
		  contentLength(0),
		  chunked(false),
		  headersParsed(false),
		  complete(false),
		  fd_(fd)
{}

Request::~Request()
{}

void Request::setMethodFromStr(const std::string &s)
{
	std::map<std::string, e_methods> methodsParser = Location::getMethodsParser();
	if (methodsParser.find(s) != methodsParser.end())
		method = methodsParser[s];
	//else
	//	throw Location::LocException::WrongMethod();
}

e_methods Request::getMethod() const
{ return method; }

const std::string &Request::getReqTarget() const
{ return reqTarget; }

const std::string &Request::getVersion() const
{ return version; }

const std::map<std::string, std::string> &Request::getHeaders() const
{ return headers; }

const std::string &Request::getBody() const
{ return body; }

bool Request::isComplete() const
{ return complete; }

void Request::parse_headers(std::string str)
{
	std::string line;
	std::vector<std::string> v;
	size_t newPos;
	int line_num = 0;

	while ((newPos = str.find_first_of('\r')) != std::string::npos)
	{
		line.clear();
		line = str.substr(0, newPos);
		str.erase(0, newPos + 2);
		++line_num;
		if (line_num == 1)
		{
			v = ft::split(line, ' ');
			if (v.size() != 3)
				throw InvalidData(line_num);
			setMethodFromStr(v[0]);
			reqTarget = v[1];
			version = v[2];
		} else
		{
			if ((newPos = line.find_first_of(':')) == std::string::npos)
				throw InvalidData(line_num);
			std::string tmp = line.substr(0, newPos);
			ft::tolower(tmp);
			headers[tmp] = line.substr(newPos + 2, line.size() - 1);
		}
	}
	std::map<std::string, std::string>::iterator it;
	for (it = headers.begin(); it != headers.end(); ++it)
	{
		if (it->first == "content-length")
			contentLength = ft::to_num(it->second);
		if (it->first == "transfer-encoding" && it->second == "chunked")
			chunked = true;
	}
}

int Request::parse_chunk(const int fd)
{
	int ret;
	char buffer[2049];

	if (!contentLength)
	{
		while (raw_request.find("\r\n") == std::string::npos)
		{
			if ((ret = read(fd, buffer, 1)) > 0)
			{
				buffer[ret] = 0x0;
				raw_request += buffer;
			} else
				return ret;
		}
		contentLength = ft::to_num(raw_request, true) + 2;
		raw_request.clear();
	}
	if (contentLength)
	{
		while (raw_request.size() != contentLength)
		{
			if ((ret = read(fd, buffer, (contentLength - raw_request.size()) % 1024)) > 0)
			{
				buffer[ret] = 0x0;
				raw_request += buffer;
			} else
				return ret;
		}
		if (contentLength == 2)
			complete = true;
		else
			body += raw_request.substr(0, raw_request.size() - 2);
		raw_request.clear();
		contentLength = 0;
	}
	return ret;
}

int Request::parse_body(const int fd)
{
	int ret;
	char buffer[2049];

	if (!chunked)
	{
		while (raw_request.size() != contentLength)
		{
			if ((ret = read(fd, buffer, (contentLength - raw_request.size()) % 1024)) > 0)
			{
				buffer[ret] = 0x0;
				raw_request += buffer;
			} else
				return ret;
		}
		body = raw_request;
		raw_request.clear();
		complete = true;
		return ret;
	}
//	else {
	while (!complete)
	{
		if (!(ret = parse_chunk(fd)))
			return ret;
	}
	return ret;
//	}
}

int Request::receive()
{
	int ret;
	char buffer[2049];
	size_t i;

	if (!headersParsed)
	{
		while ((ret = recv(fd_, buffer, 2048, MSG_PEEK | MSG_DONTWAIT)) > 0)
//		while ((ret = read(fd_, buffer, 1)) > 0)
		{
			buffer[ret] = 0x0;
			raw_request += buffer;
			//	std::cout << raw_request << std::endl;
			if ((i = raw_request.find("\r\n\r\n")) != std::string::npos)
			{
				parse_headers(raw_request.substr(0, i + 2));
				raw_request.clear();
				read(fd_, buffer, i + 4);
				headersParsed = true;
				break;
			}
		}
	}
	if (headersParsed)
	{
		if (contentLength || chunked)
			return parse_body(fd_);
		else
			complete = true;
	}
	return ret;
}

void Request::clear()
{
	method = OTHER;
	reqTarget.clear();
	headers.clear();
	body.clear();
	version.clear();
	raw_request.clear();
	contentLength = 0;
	headersParsed = false;
	complete = false;
	chunked = false;
}

static bool methodNotAllowed(e_methods method, const std::vector<e_methods> &methodsAllowed)
{
	for (size_t i = 0; i < methodsAllowed.size(); ++i)
	{
		if (methodsAllowed[i] == method)
			return (false);
	}
	return (true);
}

int Request::isValid(Location &location)
{
	if (body.size() > location.getMaxBody())
		return 413;
	if (method == OTHER)
		return 501;
	if ((methodNotAllowed(method, location.getMethods())))
		return 405;
	return 0;
}

std::ostream &operator<<(std::ostream &os, const Request &request)
{
	os << " method: " << request.getMethod() << std::endl;
	os << " reqTarget: " << request.getReqTarget() << std::endl;
	os << " version: " << request.getVersion() << std::endl;
	os << " HEADERS: " << std::endl << request.getHeaders() << std::endl;
	os << " body: " << request.getBody() << std::endl;
	os << " complete: " << std::boolalpha << request.isComplete() << std::endl;
	return os;
}