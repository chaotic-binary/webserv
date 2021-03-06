//
// Created by Mahmud Jego on 3/4/21.
//

#include "Response.hpp"

Response::Response(int fd, const Request &req, const ServConfig &config)
: _fd(fd), _req(req), _config(config)
{
	if(selectLocation())
	{
		std::cerr << "err 404" << std::endl;
		exit(404); // temp
	}
}

int Response::checkSource()
{

	return 0;
}

int Response::selectLocation()
{
	const std::vector<Location> loc = _config.getLocations();
	std::vector<Location>::const_iterator it = loc.cbegin();
	int res = 0;

	for (; it != loc.cend(); it++)
	{

	}
	return (0);
}
