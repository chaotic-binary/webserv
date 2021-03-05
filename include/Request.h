#pragma once

# include "Parser.hpp"
# include <sstream>
#include <ostream>

class Request {
private:
	e_methods											method;
	std::string		 									reqTarget;
	std::string 										version;
	std::map< std::string, std::vector<std::string> >	headers;
	std::string 										body;
	Request();

public:
	Request(const std::string &);
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

	class InvalidData: public std::logic_error {
	private: InvalidData();
	public: InvalidData(int line) : std::logic_error("Wrong number of arguments: line:" + ft::to_str(line)){};
	};
};

std::ostream &operator<<(std::ostream &os, const Request &request);


