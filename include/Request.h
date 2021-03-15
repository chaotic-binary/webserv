#pragma once

# include "utils.h"
# include "Location.h" //TODO::move enum from Location
# include <sstream>
# include <ostream>
# include <unistd.h>

enum e_client_status
{
	CLOSE_CONNECTION, READY_TO_READ, READY_TO_SEND
};

/*enum e_header
{
	ACCEPT_CHARSETS,
	ACCEPT_LANGUAGE,
	AUTHORIZATION,
	CONTENT_LANGUAGE,
	CONTENT_LENGTH,
	CONTENT_TYPE,
	DATE,
	HOST,
	REFERER,
	TRANSFER_ENCODING,
	USER_AGENT*/
	/*,

	SERVER,
	WWW_AUTHENTICATE,
	ALLOW,
	CONTENT_LOCATION,
	RETRY_AFTER,
	LOCATION,
	LAST_MODIFIED

};*/

class Request {
private:
	e_methods							method;
	std::string		 					reqTarget;
	std::string 						version;
	std::map<std::string, std::string>	headers;
	std::string 						body;
	std::string 						raw_request;
	size_t								contentLength;
	bool								chunked;
	bool								headersParsed;
	bool								complete;
	const int 							fd_;

	void	setMethodFromStr(const std::string &);
	void	parse_headers(std::string);
	int		parse_body(const int fd);
	int		parse_chunk(const int fd);

	Request();

public:
	Request(const int fd);
	virtual ~Request();

	e_methods									getMethod() const;
	const std::string							&getReqTarget() const;
	const std::string 							&getVersion() const;
	const std::map<std::string, std::string>	&getHeaders() const;
	const std::string							&getBody() const;
	bool										isComplete() const;

	class InvalidData: public std::logic_error {
	private: InvalidData();
	public: InvalidData(int line) : std::logic_error("Wrong number of arguments: line:" + ft::to_str(line)){};
	};

	int		receive();
	void	clear();
	bool 	isValid(Location &location);
};

std::ostream &operator<<(std::ostream &os, const Request &request);


