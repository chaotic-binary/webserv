#include "method_utils.h"
#include <map>
#include <mimeTypes.h>
#include <response.h>
#include "Parser.h"

std::string checkSource(const Location &location, const std::string &reqTarget) {
	std::string pathObj;
	if (reqTarget == "/")
		pathObj = location.getRoot() + location.getIndex();
	else
		pathObj = location.getRoot() + reqTarget.substr(1);

	std::ifstream file(pathObj);
	if (!file) {
		throw RespException(Response(404));
	}
	file.close();
	if (location.getAutoindex()) {
		throw RespException(Response(403));
	}
	return (pathObj);
}

