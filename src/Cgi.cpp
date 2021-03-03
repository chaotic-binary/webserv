//
// Created by Mahmud Jego on 3/3/21.
//

#include "Cgi.hpp"

Cgi::Cgi(const std::map<std::string, std::vector<std::string> > &headers)
{
	this->env[AUTH_TYPE] = 0x0; //TODO:: ?
	this->env[CONTENT_LENGTH] = 0x0; //TODO:: ?
	this->env[CONTENT_TYPE] = 0x0; //TODO:: ?
	this->env[GATEWAY_INTERFACE] = 0x0; //TODO:: ?
	this->env[PATH_INFO] = 0x0; //TODO:: ?
	this->env[PATH_TRANSLATED] = 0x0; //TODO:: ?
	this->env[QUERY_STRING] = 0x0; //TODO:: ?
	this->env[REMOTE_ADDR] = 0x0; //TODO:: ?
	this->env[REMOTE_IDENT] = 0x0; //TODO:: ?
	this->env[REMOTE_USER] = 0x0; //TODO:: ?
	this->env[REQUEST_METHOD] = 0x0; //TODO:: ?
	this->env[REQUEST_URI]  = this->getRequestUri(headers);
	this->env[SCRIPT_NAME] = 0x0; //TODO:: ?
	this->env[SERVER_NAME] = this->getServerName();
	this->env[SERVER_PORT] = 0x0; //TODO:: ?
	this->env[SERVER_PROTOCOL] = 0x0; //TODO:: ?
	this->env[SERVER_SOFTWARE] = 0x0; //TODO:: ?
	this->env[17] = 0x0;
}

char *Cgi::getServerName()
{
	return (strdup("SERVER_NAME=MyLazyServer"));
}

char *Cgi::getRequestUri(const std::map<std::string, std::vector<std::string> > &headers)
{
	for(std::map<std::string, std::vector<std::string> >::const_iterator it = headers.cbegin(); it != headers.cend(); it++)
	{
		if (it->first == METHOD_GET || it->first == METHOD_HEAD || it->first == METHOD_POST
		|| it->first == METHOD_PUT || it->first == METHOD_DELETE || it->first == METHOD_CONNECT
		|| it->first == METHOD_OPTIONS || it->first == METHOD_TRACE || it->first == METHOD_PATCH)
			return (strdup(it->second[1].c_str()));
	}
	return NULL;
}
