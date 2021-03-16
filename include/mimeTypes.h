#pragma once
#include <string>
#include <map>
const std::map<std::string, std::string>& initMimeType(std::map<std::string, std::string>& mimeTypes);
std::string getMimeType(const std::string &file);
