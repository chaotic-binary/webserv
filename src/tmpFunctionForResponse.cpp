//
// Created by Mahmud Jego on 3/4/21.
//

#include "king_Response.hpp"
#include "Request.h"
#include "mimeTypes.h"

std::string getMimeType(const std::string& file)
{
	static std::map<std::string, std::string> mimeTypes;
	if (mimeTypes.empty())
		initMimeType(mimeTypes);
	std::string exp =  &file.c_str()[file.rfind('.')];
	std::string res;
	if (mimeTypes.count(exp))
	{
		res = mimeTypes[exp];
	}
	else
		res = "text/plain";
	return res;
}

void tmpFunctionForResponse(int fd, const Request &req, const ServConfig& config)
{
	king_Response response1(fd, req, config);

	response1.generateBody();
	response1.generateHeaders();

	std::string res = response1.getRespone();
	int ret = send(fd, res.c_str(), res.length(), 0);
	if (ret == -1)
		exit(13);
}

