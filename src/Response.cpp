//
// Created by Mahmud Jego on 3/4/21.
//

#include "Response.hpp"

Response::Response(int fd, const Request &req, const ServConfig &config)
: _fd(fd), _req(req), _config(config)
{
	this->selectLocation();
}

std::string Response::checkSource()
{
	if (_req.getReqTarget() == "/")
		_pathObj = _config.getLocations()[_location].getRoot()
				 + _config.getLocations()[_location].getIndex();
	else
		_pathObj = _config.getLocations()[_location].getRoot()
				 + &_req.getReqTarget().c_str()[1];

	std::ifstream file(_pathObj);

	if (!file)
	{
		_pathObj = getcwd(0x0, 0);
		_pathObj += "/config/error_pages/404.html";
		return (NOT_FOUND);
	}
	if (_config.getLocations()[_location].getAutoindex())
	{
		_pathObj = getcwd(0x0, 0);
		_pathObj += "/config/error_pages/403.html";
		return (FORBIDDEN);
	}
	file.close();
	return (OK);
}

void Response::selectLocation()
{
	const std::vector<Location> loc = _config.getLocations();
	std::vector<Location>::const_iterator it = loc.cbegin();
	int maxCoincidence = 0;
	//TODO: rewrite
	for (int res = 0; it != loc.cend(); it++, res++)
	{
		if (_req.getReqTarget().compare(0, it->getPath().size(), it->getPath()) == 0
		&& it->getPath().size() > maxCoincidence)
		{
			this->_location = res;
			maxCoincidence = it->getPath().size();
		}
	}
	status = checkSource();
}

void Response::generateHeaders()
{
	this->_responseHeaders << "HTTP/1.1 " + status
	<< ((status == "200") ? "OK"
				  :  (status == "403") ? "Forbidden"
				  :  (status == "404") ? "Not Found" : "") << " \r\n"

	<< "Version: HTTP/1.1\r\n"
	<< "Content-Type: " << getMimeType(_pathObj) << "; charset=utf-8\r\n"
	<< "Content-Length: " << _responseBody.str().length()
	<< "\r\n\r\n";
}

void Response::generateBody()
{
	std::ifstream file(_pathObj);
	_responseBody << file.rdbuf();

	file.close();
}

std::string Response::getRespone()
{
	std::string res = _responseHeaders.str() + _responseBody.str();
	_responseHeaders.clear();
	_responseBody.clear();
	return res;
}
