#include <fstream>
#include "ServConfig.h"
#include "response.h"
#include "Request.h"

Response PutGenerator(const Request &request, const ServConfig &config)
{
	Response rsp(201);
	std::string reqTarget = request.getReqTarget();
	const Location &location = config.getLocation(request.getReqTarget());
	std::string absPath = location.getRoot() + reqTarget.erase(0, location.getPath().size() + 1);;
	int i = location.getRoot().size();

	while ((i = absPath.find("/", i)) != -1) {
		std::string dir = absPath.substr(0, i++);
		mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		printf("%s\n", strerror(errno));
		if (errno == EACCES || errno == EROFS
		|| errno == ENOSPC || errno == ENAMETOOLONG)
			throw Response(403);
	}
	struct stat		sb;
	std::ofstream	of;
	if(lstat(absPath.c_str(), &sb) == 0 && S_ISREG(sb.st_mode))
		of.open(absPath, std::ios::app);
	else
		of.open(absPath);
	size_t n = 1;
	while (of.fail())
		of.open(absPath + '_' + std::to_string(n++));
	of << request.getBody();
	of.close();
	return rsp;
}
