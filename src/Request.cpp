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
		: method(OTHER), contentLength(0), chunked(false), headersParsed(false), complete(false), fd_(fd) {
//	if (headersList.empty())
//		headersListInit();
}

Request::~Request() {}

e_methods Request::getMethod() const { return method; }

void Request::setMethodFromStr(const std::string &s) {
	std::map<std::string, e_methods> methodsParser = Location::getMethodsParser();
	if (methodsParser.find(s) != methodsParser.end())
		method = methodsParser[s];
	else
		throw Location::LocException::WrongMethod();
}

const std::string	&Request::getReqTarget() const { return reqTarget; }
const std::string	&Request::getVersion() const { return version; }
const std::map< std::string, std::string >	&Request::getHeaders() const { return headers; }
const std::string	&Request::getBody() const { return body; }
bool	Request::isComplete() const { return complete; }
//const std::vector<std::string>	&Request::getHeadersList() { return headersList; }

void Request::parse_headers(std::string str) {
	std::string					line;
	std::vector<std::string>	v;
	int							line_num = 0;
	size_t						newPos;

	while ((newPos = str.find_first_of('\r')) != str.npos) {
		line.clear();
		line = str.substr(0, newPos);
		str.erase(0, newPos + 2);
		++line_num;
		if (line_num == 1) {
			v = ft::split(line, ' ');
			if (v.size() != 3)
				throw InvalidData(line_num);
			setMethodFromStr(v[0]);
			reqTarget = v[1];
			version = v[2];
		}
		else {
			newPos = line.find_first_of(':');
			if (newPos == line.npos)
				throw InvalidData(line_num);
			std::string tmp = line.substr(0, newPos);
			ft::tolower(tmp);
			headers[tmp] = line.substr(newPos + 2, line.size() - 1);
		}
	}
	std::map< std::string, std::string >::iterator it;
	for (it = headers.begin(); it != headers.end(); ++it) {
		if (it->first == "content-length")
			contentLength = ft::to_num(it->second);
		if (it->first == "transfer-encoding" && it->second == "chunked")
			chunked = true;
	}
}

int Request::parse_chunk(const int fd) {
	int		ret;
	char	buffer[2049];

	if (!contentLength) {
		while (raw_request.find("\r\n") == raw_request.npos) {
			if ((ret = read(fd, buffer, 1)) > 0) {
				buffer[ret] = 0x0;
				raw_request += buffer;
			} else
				return ret;
		}
		contentLength = ft::to_num(raw_request, true) + 2;
		raw_request.clear();
	}
	if (contentLength) {
		while (raw_request.size() != contentLength) {
			if ((ret = read(fd, buffer, (contentLength - raw_request.size()) % 1024)) > 0) {
				buffer[ret] = 0x0;
				raw_request += buffer;
			}
			else
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
	int		ret;
	char	buffer[2049];

	if (!chunked) {
		while (raw_request.size() != contentLength) {
			if ((ret = read(fd, buffer,(contentLength - raw_request.size()) % 1024)) > 0)
			{
				buffer[ret] = 0x0;
				raw_request += buffer;
			}
			else
				return ret;
		}
		body = raw_request;
		raw_request.clear();
		complete = true;
		return ret;
	}
//	else {
		while (!complete) {
			if (!(ret = parse_chunk(fd)))
				return ret;
		}
		return ret;
//	}
}

int Request::receive() {
	int		ret;
	char	buffer[2049];
	size_t	i;

	if (!headersParsed)
	{
		while ((ret = read(fd_, buffer, 2048)) > 0)
		{
			buffer[ret] = 0x0;
			raw_request += buffer;
			if ((i = raw_request.find("\r\n\r\n")) != raw_request.npos)
			{
				parse_headers(raw_request.substr(0, i + 2));
				raw_request.clear();
				lseek(fd_, i + 3, SEEK_SET); //
				headersParsed = true;
				break;
			}
		}
	}
	if (headersParsed) {
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

std::ostream &operator<<(std::ostream &os, const Request &request) {
	os << " method: " << request.getMethod() << std::endl;
	os << " reqTarget: " << request.getReqTarget() << std::endl;
	os << " version: " << request.getVersion() << std::endl;
	os << " HEADERS: " << std::endl << request.getHeaders() << std::endl;
	os << " body: " << request.getBody() << std::endl;
	os << " complete: " << std::boolalpha << request.isComplete() << std::endl;
	return os;
}