#include <fstream>
#include "ServConfig.h"
#include "response.h"
#include "Request.h"
#include <cstdio>

Response DeleteGenerator(const Request &request, const ServConfig &config)
{
	Response rsp(204);
	std::string reqTarget = request.getReqTarget();
	const Location &location = config.getLocation(request.getReqTarget());
	std::string absPath = location.getRoot() + reqTarget.erase(0, location.getPath().size() + 1);;

	if(remove(absPath.c_str()) == 0)
		return rsp;
	else if(errno == EACCES || errno == ENOTDIR || errno == EROFS)
		return Response(403);
	return Response(500);
}
