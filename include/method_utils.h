#pragma once
#include <string>
#include "Location.h"
#include "ServConfig.h"

const Location &getLocation(const std::string& path, const ServConfig &config);
std::string checkSource(const Location &location, const std::string &reqTarget);
