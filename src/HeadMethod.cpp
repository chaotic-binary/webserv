#include <fstream>
#include <method_utils.h>
#include <mimeTypes.h>
#include "GetMethod.h"
#include "ServConfig.h"

Response HeadGenerator(const Request &request, const ServConfig &config) {
	Response rsp(200);
	const Location &location = config.getLocation(request.getReqTarget());
	const std::string obj = checkSource(location, request.getReqTarget());

	std::ifstream file(obj);
	std::stringstream ss;
	ss << file.rdbuf();
	file.close();

	rsp.SetHeader("Content-Length", std::to_string(ss.str().length()));
	rsp.SetHeader("content-type", getMimeType(obj));
	return rsp;

}
