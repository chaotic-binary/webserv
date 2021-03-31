#include <fstream>
#include <dirent.h>
#include "mimeTypes.h"
#include "ServConfig.h"
#include "response.h"
#include "Request.h"
#include <sys/stat.h>

std::string generateIndexPage(const char* obj) {
	std::string res, tmp;
	struct dirent *dir_el;
	DIR		*dir;

	if ((dir = opendir(obj))) {
		res += "<title>KinGinx AutoIndex</title><style>a{text-decoration: none; color: black; font-size: 25px;}"
		 "body {"
		"	background: rgb(238,174,202);"
		"	background: radial-gradient(circle, rgba(238,174,202,1) 0%, rgba(148,187,233,1) 100%);}"
		"ul {"
		"	display: flex;"
		"	flex-wrap: wrap;"
		"	flex-direction: column;"
//		"	border: 2px solid blue;"
		"	border-radius: 5px;"
		"	text-align: center;"
		"	padding-left: 0;}"
		"li {"
		"	display: block;"
		"	border-bottom: 1px solid #673ab7;"
		"	padding-bottom: 5px;}"
//		"h1 footer {"
//		"	color:b; }"
//   ".dir li:last-child {border-bottom: none}"
   "</style>";
		res += "<ul class=\"dir\"><li><h1>KinGinx Index</h1><p></li>";
		while ((dir_el = readdir(dir))) {
			res += "<li><a href=\"/";
			res += dir_el->d_name;
			res += "\">";
			res += dir_el->d_name;
			res += "</a></li>";
			tmp = dir_el->d_name;
			if (!tmp.compare(0, 6, "index.")) {
				closedir(dir);
				return std::string();
			}
		}
		res += "</ul><footer><small>&copy; KinGinx team, 2021-2077</small></footer>";
	}
	else {
		throw RespException(Response(404));
	}
	closedir(dir);
	return res;
}

static std::string checkSource(const Location &location, std::string reqTarget,  bool cgi = false)
{
	std::string	pathObj;
	struct stat	sb;

	pathObj = location.getRoot();
	if (reqTarget != location.getPath())
		pathObj += reqTarget.erase(0, location.getPath().size());
	stat(pathObj.c_str(), &sb);
	if (S_ISDIR(sb.st_mode)) {
		pathObj += (pathObj.back() != '/') ? "/" : "";
		pathObj += cgi ? location.getCgiIndex() : location.getIndex(); //TODO: what if cgiindex or index is empty?!
	}
	std::ifstream file(pathObj);
	if (!file && !location.getAutoindex() && location.getIndex().empty())
		throw RespException(Response(404));
	if (!location.getAutoindex() && !location.getIndex().empty())
		throw RespException(Response(403));
	file.close();
	return (pathObj);
}

static std::string getTimeFormat(const char *obj)
{
	struct stat sb;
	lstat(obj, &sb);
	char buff[1024];
	tm date;

	strptime(std::to_string(sb.st_mtimespec.tv_sec).c_str(), "%s", &date);
	size_t ret = strftime(buff, 1024, "%a, %d %b %Y %T GMT", &date);
	buff[ret] = '\0';
	return buff;
}

Response GetGenerator(const Request &request, const ServConfig &config) {
	Response rsp(200);
	const Location &location = config.getLocation(request.getReqTarget());
	const std::string obj = checkSource(location, request.getReqTarget());

	std::ifstream file(obj);
	std::stringstream ss;
	ss << file.rdbuf();
	file.close();
	std::string bodyAutoIndex = generateIndexPage(location.getRoot().c_str());
	if (location.getAutoindex() && request.getReqTarget() == location.getPath() && !bodyAutoIndex.empty())
		rsp.SetBody(bodyAutoIndex);
	else
		rsp.SetBody(ss.str());

	rsp.SetHeader("Last-Modified", getTimeFormat(obj.c_str()));
	size_t i =  ss.str().find("<html lang=");
	if (i != std::string::npos)
		rsp.SetHeader("Content-Language", ss.str().substr(i + 12,  2));
	rsp.SetHeader("content-type", getMimeType(obj));
	return rsp;
}
