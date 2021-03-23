#include "method_utils.h"
#include <algorithm>
#include "response.h"
#include "Cgi.h"

Response PostGenerator(const Request &request, const ServConfig &config) {
	Response rsp(200);
	const Location& location = config.getLocation(request.getReqTarget());
	//TODO: is this cgi Correct?!
	//std::string path = checkSource(location, request.getReqTarget(), true);
	//std::cerr << "!!!!"  << path ;
	__unused	const std::vector<std::string> &cgi_extentions = location.getCgiExtensions();

//	if (cgi_extentions.empty() ||
//	cgi_extentions.end() == find(cgi_extentions.begin(), cgi_extentions.end(), path.substr(path.rfind('.'))))
//		return Response(404);
	std::string	path = request.getReqTarget();
	path.erase(0, location.getPath().size() + 1);
	path = location.getRoot() + path;

	EnvironMap tmp = CgiGenerateEnv(request, config);
	std::string total = CgiEx(location.getCgiPath(), path, request.getBody(), tmp);
	rsp.SetBody(total);
	//rsp.SetHeader("content-type", getMimeType(obj));
	return rsp;
}