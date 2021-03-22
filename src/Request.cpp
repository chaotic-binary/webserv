#include <libc.h>
#include "Request.h"

#define BUFFER_SIZE 10000

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
	  fd_(fd) {}

Request::~Request() {}

void Request::setMethodFromStr(const std::string &s) {
	std::vector<std::string> methodsParser = Location::getMethodsParser();
	for (size_t i = 0; i < methodsParser.size(); ++i) {
		if (s == methodsParser[i])
			method = static_cast<e_methods>(i);
	}
}

e_methods Request::getMethod() const { return method; }

const std::string &Request::getReqTarget() const { return reqTarget; }

const std::string &Request::getVersion() const { return version; }

const std::map<std::string, std::string> &Request::getHeaders() const { return headers; }

const std::string &Request::getBody() const { return body; }

const std::string &Request::GetUri() const {
	return uri_;
}

const std::string &Request::getHeader(const std::string &title) const {
	const static std::string empty;
	try {
		return this->getHeaders().at(ft::tolower(title));
	} catch (const std::out_of_range &) {
		return empty;
	}
}

const std::string &Request::GetQueryString() const {
	return queryString;
}

bool Request::isComplete() const { return complete; }

void Request::parse_headers(std::string str) {
	std::string line;
	std::vector<std::string> v;
	size_t newPos;
	int line_num = 0;

	while ((newPos = str.find_first_of('\r')) != std::string::npos) {
		line.clear();
		line = str.substr(0, newPos);
		str.erase(0, newPos + 2);
		if (!line_num && line.empty())
			continue;
		++line_num;
		if (line_num == 1) {
			v = ft::split(line, ' ');
			if (v.size() != 3)
				throw InvalidFormat(line_num);
			if (v[2].find("HTTP/") != 0)
				throw InvalidFormat(line_num);
			setMethodFromStr(v[0]);
			reqTarget = v[1];
			version = v[2];
		} else {
			if ((newPos = line.find_first_of(':')) == std::string::npos)
				throw InvalidFormat(line_num);
			std::string tmp = line.substr(0, newPos);
			tmp = ft::tolower(tmp);
			if (headers.count(tmp)) {
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
	for (it = headers.begin(); it != headers.end(); ++it) {
		if (it->first == "content-length")
			contentLength = ft::to_num(it->second);
		if (it->first == "transfer-encoding" && it->second == "chunked")
			chunked = true;
	}
	uri_ = reqTarget;
	size_t i;
	if ((i = reqTarget.find('?')) != std::string::npos) {
		queryString = reqTarget.substr(i + 1, reqTarget.size());
		reqTarget.erase(i, reqTarget.size());
	}
}

int Request::parse_chunk(const int fd, bool &read_activated) {
	int ret;
	char buffer[BUFFER_SIZE + 1];
	size_t i;

	if (!contentLength) {
		if (raw_request.find("\r\n") == std::string::npos) {
			if (read_activated)
				return -4;
			if ((ret = read(fd_, buffer, BUFFER_SIZE)) > 0) {
				buffer[ret] = 0x0;
				//std::cout << "fd: " << fd_ << " rr:" << raw_request << " :rr" << std::endl;
				raw_request += buffer;
				//std::cout << "rr: " << raw_request << " :rr" << std::endl;
				read_activated = true;
			} else
				return ret;
		}
		if ((i = raw_request.find("\r\n")) != std::string::npos) {
			contentLength = ft::to_num(raw_request, true) + 2;
			raw_request.erase(0, i + 2);
		}
	}
	if (contentLength) {
		if (raw_request.size() < contentLength) {
			if (read_activated)
				return -4;
			size_t read_len = (contentLength - raw_request.size()) % BUFFER_SIZE;
			read_len = read_len ? read_len : BUFFER_SIZE;
			if ((ret = read(fd, buffer, read_len)) > 0) {
				buffer[ret] = 0x0;
				raw_request += buffer;
				//std::cout << "SIZE = " <<  raw_request.size() << std::endl;
			} else
				return ret;
		}
		if (raw_request.size() >= contentLength) {
			if (contentLength == 2)
				complete = true;
			else
				body += raw_request.substr(0, contentLength - 2);
			//std::cout << "BOSY SIZE = " <<  body.size() << std::endl;
			raw_request.erase(0, contentLength);
			contentLength = 0;
			return 1;
		}
	}
	return 1;
}

int Request::parse_body(const int fd, bool read_activated) {
	int ret = 1;
	char buffer[BUFFER_SIZE + 1];

	if (!chunked) {
		if (raw_request.size() < contentLength) {
			if (read_activated)
				return 1;
			size_t read_len = (contentLength - raw_request.size()) % BUFFER_SIZE;
			read_len = read_len ? read_len : BUFFER_SIZE;
			if ((ret = read(fd, buffer, read_len)) > 0) {
				buffer[ret] = 0x0;
				raw_request += buffer;
				read_activated = true;
			} else
				return ret;
		}
		if (raw_request.size() >= contentLength) {
			body = raw_request.substr(0, contentLength);
			raw_request.erase(0, contentLength);
			complete = true;
			return 1;
		}
	} else
		while (!complete) {
			ret = parse_chunk(fd, read_activated);
			if (ret <= 0)
				return (ret == -4) ? 1 : ret;
		}
	return ret;
}

int Request::receive() {
	assert(complete == false);
	int ret = 1;
	char buffer[BUFFER_SIZE + 1];
	size_t i;
	bool read_activated = false;

	if (!headersParsed) {
		if (raw_request.find("\r\n\r\n") == std::string::npos) {
			if ((ret = read(fd_, buffer, BUFFER_SIZE)) > 0) {
				read_activated = true;
				buffer[ret] = 0x0;
				raw_request += buffer;
				bzero(buffer, BUFFER_SIZE);
				//std::cout << "fd: " << fd_ << " RAW<" << raw_request << ">RAW" << std::endl;
			} else
				return ret;
		}
		if ((i = raw_request.find("\r\n\r\n")) != std::string::npos) {
			parse_headers(raw_request.substr(0, i + 2));
			raw_request.erase(0, i + 4);
			//	read(fd_, buffer, i + 4);
			headersParsed = true;
			//std::cout << "fd " << fd_ << ": !Headers parsed!" << std::endl;
		}
	}
	if (headersParsed) {
		if (!contentLength && !chunked)
			complete = true;
		else
			return parse_body(fd_, read_activated);
	}
	return ret;
}

void Request::clear() {
	method = OTHER;
	reqTarget.clear();
	queryString.clear();
	headers.clear();
	body.clear();
	version.clear();
	//raw_request.clear();
	uri_.clear();
	contentLength = 0;
	headersParsed = false;
	complete = false;
	chunked = false;
}

std::ostream &operator<<(std::ostream &os, const Request &request) {
	os << " method: " << ((request.getMethod() == OTHER) ? "OTHER" : ft::to_str(request.getMethod())) << std::endl;
	os << " reqTarget: " << request.getReqTarget() << std::endl;
	os << " queryString: " << request.GetQueryString() << std::endl;
	os << " version: " << request.getVersion() << std::endl;
	os << " HEADERS: " << std::endl << request.getHeaders() << std::endl;
	os << "URI: " << request.GetUri() << std::endl;
	os << " body: " << request.getBody() << std::endl;
	os << " complete: " << std::boolalpha << request.isComplete() << std::endl;
	return os;
}
