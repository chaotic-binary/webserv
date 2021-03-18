#include <fstream>
#include <method_utils.h>
#include <mimeTypes.h>
#include "GetMethod.h"
#include "ServConfig.h"

Response PutGenerator(const Request &request, const ServConfig &config)
{
	Response rsp(201);
	std::vector<std::string> dirs;

	std::string g = request.getReqTarget().substr(1);
	int i;
	while ((i = g.find("/")) != -1)
	{
		dirs.push_back(g.substr(0, i));
		g = g.substr(0, i);
	}
/*	std::ifstream file(obj);
	std::stringstream ss;
	ss << file.rdbuf();
	file.close();*/
	config.getHost();
	return rsp;

}
