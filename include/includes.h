//
// Created by Mahmud Jego on 3/2/21.
//
#pragma once

#ifndef INCLUDES_H
#define INCLUDES_H

# include <iostream>
# include <fstream>
# include <regex>
# include <sstream>
# include <ostream>
# include <sys/socket.h>
# include <unistd.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <sstream>
# include <vector>
# include <algorithm>
# include <map>
# include <stack>
# include <deque>
# include <list>

#include "defines.h"
#include "ServConfig.h"

enum eEnvCgi {
	AUTH_TYPE,
	CONTENT_LENGTH,
	CONTENT_TYPE,
	GATEWAY_INTERFACE,
	PATH_INFO,
	PATH_TRANSLATED,
	QUERY_STRING,
	REMOTE_ADDR,
	REMOTE_IDENT,
	REMOTE_USER,
	REQUEST_METHOD,
	REQUEST_URI,
	SCRIPT_NAME,
	SERVER_NAME,
	SERVER_PORT,
	SERVER_PROTOCOL,
	SERVER_SOFTWARE
};

#endif //INCLUDES_H
