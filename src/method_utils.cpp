#include "method_utils.h"
#include <map>
#include <mimeTypes.h>
#include <response.h>
#include "Parser.hpp"


const Location &getLocation(const std::string &path, const ServConfig &config) {
	//TODO:: move in ServConfig class? @ttamesha
	const std::vector<Location> &locations = config.getLocations();
	std::vector<Location>::const_iterator it = locations.cbegin();
	size_t maxCoincidence = 0;
	int locationIndex = -1;

	// TODO: refact select location
	for (int res = 0; it != locations.cend(); it++, res++) {
		if (path.compare(0, it->getPath().size(), it->getPath()) == 0
			&& it->getPath().size() > maxCoincidence) {
			locationIndex = res;
			maxCoincidence = it->getPath().size();
		}
	}
	if (locationIndex == -1)
		throw RespException(Response(404));
	return locations[locationIndex];
//	status = checkSource();
}
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

