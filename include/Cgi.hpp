//
// Created by Mahmud Jego on 3/3/21.
//

#ifndef CGI_HPP
#define CGI_HPP

#include "includes.h"

class Cgi {
public:
	Cgi(const std::map<std::string, std::vector<std::string> >& headers);

//	char *initEnv(const std::map<std::string, std::vector<std::string> > &headers, int header);

	char*	getServerName();
	char*	getRequestUri(const std::map<std::string, std::vector<std::string> > &headers);
private:
	char	*env[18];
};


#endif //CGI_HPP
