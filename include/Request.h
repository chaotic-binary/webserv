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

enum e_header
{
	ACCEPT_CHARSETS,
	ACCEPT_LANGUAGE,
	ALLOW,
	AUTHORIZATION,
	CONTENT_LANGUAGE,
	CONTENT_LENGTH,
	CONTENT_LOCATION,
	CONTENT_TYPE,
	DATE,
	HOST,
	LAST_MODIFIED,
	LOCATION,
	REFERER,
	RETRY_AFTER,
	SERVER,
	TRANSFER_ENCODING,
	USER_AGENT,
	WWW_AUTHENTICATE
};

class Request {
private:
	e_methods											method;
	std::string		 									reqTarget;
	std::string 										version;
	std::map< e_header, std::vector<std::string> >		headers;
	std::string 										body;

	std::string 										raw_request;
	size_t												contentLength;
	bool												chunked;
	bool												headersParsed;
	bool												complete;
	const int 											fd_;

	static std::vector<std::string>						headersList;
	static void											headersListInit();
	void												setMethodFromStr(const std::string &);

	void	parse_headers(const std::string &str);
	int		parse_body(const int fd);
	int		parse_chunk(const int fd);

	Request();

public:
	Request(const int fd);
	virtual ~Request();
	e_methods getMethod() const;
	const std::string &getReqTarget() const;
	const std::string &getVersion() const;
	const std::map< e_header, std::vector<std::string> > &getHeaders() const;
	const std::string &getBody() const;
	bool isComplete() const;

	static const std::vector<std::string> &getHeadersList();

	class InvalidData: public std::logic_error {
	private: InvalidData();
	public: InvalidData(int line) : std::logic_error("Wrong number of arguments: line:" + ft::to_str(line)){};
	};

	int		receive();
	void	clear();
};

std::ostream &operator<<(std::ostream &os, const std::map< e_header, std::vector<std::string> > &headers);

std::ostream &operator<<(std::ostream &os, const Request &request);


