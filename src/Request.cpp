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
	std::vector<std::string> methodsParser = Location::getMethodsParser();
	for (size_t i = 0; i < methodsParser.size(); ++i)
	{
		if (s == methodsParser[i])
			method = static_cast<e_methods>(i);
	}
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
		if (!line_num && line.empty())
			continue;
		++line_num;
		if (line_num == 1)
		{
			v = ft::split(line, ' ');
			if (v.size() != 3)
				throw InvalidFormat(line_num);
			if (v[2].find("HTTP/") != 0)
				throw InvalidFormat(line_num);
			setMethodFromStr(v[0]);
			reqTarget = v[1];
			version = v[2];
		} else
		{
			if ((newPos = line.find_first_of(':')) == std::string::npos)
				throw InvalidFormat(line_num);
			std::string tmp = line.substr(0, newPos);
			ft::tolower(tmp);
			if (headers.count(tmp))
			{
				if ((tmp == "host" || tmp == "content-length"))
					throw DuplicateHeader(tmp);
				//TODO:other headers?
			}
			headers[tmp] = line.substr(newPos + 2, line.size() - 1);
		}
	}
	if (headers.find("host") == headers.end())
		throw HeaderNotPresent("host");
	std::map<std::string, std::string>::const_iterator it;
	for (it = headers.begin(); it != headers.end(); ++it)
	{
		if (it->first == "content-length")
			contentLength = ft::to_num(it->second);
		if (it->first == "transfer-encoding" && it->second == "chunked")
			chunked = true;
	}
	size_t i;
	uri_ = reqTarget;
	if ((i = reqTarget.find('?')) != std::string::npos)
	{
		queryString = reqTarget.substr(i + 1, reqTarget.size());
		reqTarget.erase(i, reqTarget.size());
	}
}
const std::string &Request::GetUri() const {
	return uri_;
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
			//std::cout << raw_request << std::endl;
			if ((i = raw_request.find("\r\n\r\n")) != std::string::npos)
			{
				parse_headers(raw_request.substr(0, i + 2));
				raw_request.clear();
				read(fd_, buffer, i + 4);
				headersParsed = true;
				break;
			} else
				read(fd_, buffer, ret);
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
	queryString.clear();
	headers.clear();
	body.clear();
	version.clear();
	raw_request.clear();
	contentLength = 0;
	headersParsed = false;
	complete = false;
	chunked = false;
}
const std::string &Request::getHeader(const std::string& title) const {
	const static std::string empty;
	try {
		return this->getHeaders().at(title);
	} catch (const std::out_of_range&) {
		return empty;
	}
}
const std::string &Request::GetQueryString() const {
	return queryString;
}

std::ostream &operator<<(std::ostream &os, const Request &request)
{
	os << " method: " << ((request.getMethod() == OTHER) ? "OTHER" : ft::to_str(request.getMethod())) << std::endl;
	os << " reqTarget: " << request.getReqTarget() << std::endl;
	os << " version: " << request.getVersion() << std::endl;
	os << " HEADERS: " << std::endl << request.getHeaders() << std::endl;
	os << " body: " << request.getBody() << std::endl;
	os << " complete: " << std::boolalpha << request.isComplete() << std::endl;
	return os;
}
