#include "Request.h"

std::vector<std::string>	Request::headersList;

void	Request::headersListInit() {
	headersList.push_back("accept-charsets:");
	headersList.push_back("accept-language:");
	headersList.push_back("allow:");
	headersList.push_back("authorization:");
	headersList.push_back("content-language:");
	headersList.push_back("content-length:");
	headersList.push_back("content-location:");
	headersList.push_back("content-type:");
	headersList.push_back("date:");
	headersList.push_back("host:");
	headersList.push_back("last-modified:");
	headersList.push_back("location:");
	headersList.push_back("referer:");
	headersList.push_back("retry-after:");
	headersList.push_back("server:");
	headersList.push_back("transfer-encoding:");
	headersList.push_back("user-agent:");
	headersList.push_back("www-authenticate:");
};

Request::Request(const int fd)
		: method(OTHER), contentLength(0), chunked(false), headersParsed(false), complete(false), fd_(fd) {
	if (!headersList.size())
		headersListInit();
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
const std::map< e_header, std::vector<std::string> >	&Request::getHeaders() const { return headers; }
const std::string	&Request::getBody() const { return body; }
bool	Request::isComplete() const { return complete; }
const std::vector<std::string>	&Request::getHeadersList() { return headersList; }

void Request::parse_headers(const std::string &str) {
	std::stringstream ss;
	ss << str;
	std::string line;
	int line_num = 0;

	while (ft::getline(ss, line)) {
		++line_num;
		std::vector<std::string> v = ft::split(line, ' ');
		if (line != "\r" && v.size() < 2)
			throw InvalidData(line_num);
		if (line_num == 1) {
			if (v.size() != 3)
				throw InvalidData(line_num);
			setMethodFromStr(v[0]);
			ft::cut_char_from_end(v[2], "\r");
			reqTarget = v[1];
			version = v[2];
		}
		else if (line == "\r") {
			break;
		} else {
			ft::tolower(v[0]);
			for (size_t h = 0; h < headersList.size(); ++h) {
				if (v[0] == headersList[h]) //{
					for (size_t i = 1; i < v.size(); ++i) {
						ft::cut_char_from_end(v[i], ",;\r");
						headers[static_cast<const e_header>(h)].push_back(v[i]);
					}
				//}
			}
		}
	}
	std::map< e_header, std::vector<std::string> >::iterator it;
	for (it = headers.begin(); it != headers.end(); ++it) {
		if (it->first == CONTENT_LENGTH)
			contentLength = ft::to_num(it->second[0]);
		if (it->first == TRANSFER_ENCODING && it->second[0] == "chunked")
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
		while (raw_request.size() != contentLength)
		{
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
	int			ret;
	char		buffer[2049];
	size_t		i;

	if (!headersParsed)
	{
		while ((ret = read(fd_, buffer, 2048)) > 0)
		{
			buffer[ret] = 0x0;
			raw_request += buffer;
			if ((i = raw_request.find("\r\n\r\n")) != raw_request.npos)
			{
				parse_headers(raw_request);
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

std::ostream &operator<<(std::ostream &os, const std::map< e_header, std::vector<std::string> > &headers) {
	std::map< e_header, std::vector<std::string> >::const_iterator it;
	std::vector<std::string> v = Request::getHeadersList();
	for (it = headers.begin(); it != headers.end(); ++it) {
		std::cout << v.at(it->first) << " => " << it->second << '\n';
	}
	return os;
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