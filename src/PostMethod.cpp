#include <method_utils.h>
#include <algorithm>
#include "PostMethod.h"
#include "Cgi.h"

Response PostGenerator(const Request &request, const ServConfig &config) {
	Response rsp(201);
	const Location& location = config.getLocation(request.getReqTarget());
	std::string path = checkSource(location, request.getReqTarget(), true);
	const std::vector<std::string>& cgi_extentions = location.getCgiExtensions();
	if(cgi_extentions.end() == find(cgi_extentions.begin(), cgi_extentions.end(), path.substr(path.rfind('.'))))
		return Response(404);
	try {
		EnvironMap tmp = CgiGenerateEnv(request, config);
		std::string total = CgiEx(location.getCgiPath(), path, request.getBody(), tmp);
		rsp.SetBody(total);
	} catch (...)
	{
		return Response(500);
	}
	//rsp.SetHeader("content-type", getMimeType(obj));
	return rsp;
}