#include "Request.h"

Request::Request() {}

Request::Request(const std::string &str) {
	std::stringstream ss;
	ss << str;
	std::string line;
	int line_num = 0;
	bool bodyStart = false;

	while (std::getline(ss, line)) {
		++line_num;
		std::vector<std::string> v = ft::split(line, ' ');
		if (line_num == 1) {
			if (v.size() != 3)
				throw InvalidData(line_num);
			setMethodFromStr(v[0]);
			setReqTarget(v[1]);
			setVersion(v[2]);
		} else if (line == "\r") {
			bodyStart = true;
		} else if (bodyStart)
			body += line;
		else {
			for (size_t i = 1; i < v.size(); ++i) {
				ft::cut_char_from_end(v[i], ",;");
				headers[v[0]].push_back(v[i]);
			}
		}
	}
}

Request::~Request() {}

e_methods Request::getMethod() const { return method; }

void Request::setMethod(e_methods method) { this->method = method; }

void Request::setMethodFromStr(const std::string &s) {
	std::map<std::string, e_methods> methodsParser = Location::getMethodsParser();
	if (methodsParser.find(s) != methodsParser.end())
		method = methodsParser[s];
	else
		throw Location::LocException::WrongMethod();
}

const std::string &Request::getReqTarget() const { return reqTarget; }
void Request::setReqTarget(const std::string &reqTarget) { this->reqTarget = reqTarget; }

const std::string &Request::getVersion() const { return version; }
void Request::setVersion(const std::string &version) { this->version = version; }

const std::map< std::string, std::vector<std::string> > &Request::getHeaders() const { return headers; }
void Request::setHeaders(const std::map<std::string, std::vector<std::string> > &headers) { this->headers = headers; }

const std::string &Request::getBody() const { return body; }

std::ostream &operator<<(std::ostream &os, const Request &request)
{
	os << "method: " << request.getMethod() << std::endl;
	os << " reqTarget: " << request.getReqTarget() << std::endl;
	os << " version: " << request.getVersion() << std::endl;
	os << " headers: " << request.getHeaders() << std::endl;
	os << " body: " << request.getBody() << std::endl;
	return os;
}