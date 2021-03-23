#include <fstream>
#include <method_utils.h>
#include <mimeTypes.h>
#include "GetMethod.h"
#include "ServConfig.h"

Response GetGenerator(const Request &request, const ServConfig &config) {
	Response rsp(200);
	const Location &location = config.getLocation(request.getReqTarget());
	const std::string obj = checkSource(location, request.getReqTarget());
	//TODO::перенести общее для всех методов

	std::ifstream file(obj);
	std::stringstream ss;
	ss << file.rdbuf();
	file.close();

	rsp.SetBody(ss.str());
	rsp.SetHeader("content-type", getMimeType(obj));
	return rsp;

}