#include "method_utils.h"
#include <algorithm>
#include "response.h"
#include "Cgi.h"

static std::map<std::string, std::string> parse_headers(const std::string &str) {
	std::vector<std::string> v;
	std::vector<std::string> lines = ft::split(str, '\n');
	std::map<std::string, std::string> headers;
	size_t newPos;

	for (size_t i = 0; i < lines.size(); ++i) {
		if ((newPos = lines[i].find_first_of(':')) == std::string::npos)
			throw std::runtime_error("Invalid format"); //TODO:where we catch it?
		std::string tmp = lines[i].substr(0, newPos);
		tmp = ft::tolower(tmp);
		/*if (headers.count(tmp)) {
			if ((tmp == "host" || tmp == "content-length"))
				throw DuplicateHeader(tmp);
		}*/
		headers[tmp] = lines[i].substr(newPos + 2, lines[i].size() - 3 - newPos);
	}
	return headers;
}

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

	size_t i = total.find("\r\n\r\n");
	std::map<std::string, std::string> headers = parse_headers(total.substr(0, i + 2));
	//if (!headers.count("status"))
	//	throw TODO:?
	rsp.SetCode(ft::to_num(headers["status"]));
	std::map<std::string, std::string>::const_iterator it;
	for (it = headers.begin(); it != headers.end(); ++it) {
		if (it->first != "status")
			rsp.SetHeader(it->first, it->second);
	}
	total.erase(0, i + 4);
	rsp.SetBody(total);
	//rsp.SetHeader("content-type", getMimeType(obj));
	return rsp;
}