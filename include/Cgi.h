#pragma once

#include <string>
#include <map>
#include "ServConfig.h"
#include "Request.h"

typedef std::map<std::string, std::string> EnvironMap;

std::map<std::string, std::string> CgiGenerateEnv(const Request& request, const ServConfig& config);

std::string CgiEx(const std::string& cgi, const std::string& script, const std::string input,
				  const EnvironMap & env_map);