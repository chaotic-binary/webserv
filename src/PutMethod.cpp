#include <fstream>
#include "GetMethod.h"
#include "ServConfig.h"
#include <sys/stat.h>

Response PutGenerator(const Request &request, const ServConfig &config)
{
	Response rsp(201);
	const Location &location = config.getLocation(request.getReqTarget());
	std::string absPath = location.getRoot() + request.getReqTarget().substr(1);
	int i = location.getRoot().size();

	while ((i = absPath.find("/", i)) != -1)
	{
		std::string dir = absPath.substr(0, i++);
		mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		printf("%s\n", strerror(errno));
		if (errno == EACCES || errno == EROFS
		|| errno == ENOSPC || errno == ENAMETOOLONG)
			throw Response(403);
	}
	std::ofstream hello(absPath);
	if (hello.fail())
		throw Response(403);
	hello << request.getBody();
	hello.close();
	return rsp;
}
