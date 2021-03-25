#include "method_utils.h"
#include <map>
#include <mimeTypes.h>
#include <response.h>
#include "Parser.h"
#include <sys/stat.h>

std::string checkSource(const Location &location, std::string reqTarget, bool cgi)
{
	std::string	pathObj;
	struct stat	sb;

	pathObj = location.getRoot();
	if (reqTarget != location.getPath())
		pathObj += reqTarget.erase(0, location.getPath().size());
	stat(pathObj.c_str(), &sb);
	if (S_ISDIR(sb.st_mode)) {
		pathObj += (pathObj.back() != '/') ? "/" : "";
		pathObj += cgi ? location.getCgiIndex() : location.getIndex(); //TODO: what if cgiindex or index is empty?!
	}
	std::ifstream file(pathObj);
	if (!file)
		throw RespException(Response(404));
	file.close();
	if (location.getAutoindex())
		throw RespException(Response(403));
	return (pathObj);
}

