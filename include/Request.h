#pragma once

# include "utils.hpp"
# include "Location.hpp" //TODO::move enum from Location
# include <sstream>
# include <ostream>
# include <unistd.h>

enum e_client_status
{
	CLOSE_CONNECTION, READY_TO_READ, READY_TO_SEND
};

/*
"Accept-Charsets:"
"Accept-Language:"
"Allow:"
"Authorization:"
"Content-Language:"
"Content-Length:"
"Content-Location:"
"Content-Type:"
"Date:"
"Host:"
"Last-Modified:"
"Location:"
"Referer:"
"Retry-After:"
"Server:"
"Transfer-Encoding:"
"User-Agent:"
"WWW-Authenticate:"
*/

class Request {
private:
	e_methods											method;
	std::string		 									reqTarget;
	std::string 										version;
	std::map< std::string, std::vector<std::string> >	headers;
	std::string 										body;

	std::string 										raw_request;
	size_t												contentLength;
	bool												chunked;
	bool												headersParsed;
	bool												complete;
	const int 											fd_;

	Request();

public:
	Request(const int fd);
	virtual ~Request();

	e_methods getMethod() const;
	void setMethod(e_methods method);
	void setMethodFromStr(const std::string &);

	const std::string &getReqTarget() const;
	void setReqTarget(const std::string &reqTarget);

	const std::string &getVersion() const;
	void setVersion(const std::string &version);

	const std::map< std::string, std::vector<std::string> > &getHeaders() const;
	void setHeaders(const std::map< std::string, std::vector<std::string> > &headers);

	const std::string &getBody() const;

	bool isComplete() const;

	class InvalidData: public std::logic_error {
	private: InvalidData();
	public: InvalidData(int line) : std::logic_error("Wrong number of arguments: line:" + ft::to_str(line)){};
	};

	int receive();

	void clear();

	void parse_headers(const std::string &str);

	int parse_body(const int fd);

	int parse_chunk(const int fd);
};

std::ostream &operator<<(std::ostream &os, const Request &request);


