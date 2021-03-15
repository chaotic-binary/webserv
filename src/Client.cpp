#include "Client.h"
#include <iostream>
#include <response.h>
#include <stdexcept>
#include <fstream>
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

const Location& getLocation(const Request &request, const ServConfig &config)
{
	const std::vector<Location>& locations = config.getLocations();
	std::vector<Location>::const_iterator it = locations.cbegin();
	size_t maxCoincidence = 0;
	int locationIndex = -1;
	
	// TODO: refact select location
	for (int res = 0; it != locations.cend(); it++, res++) 
	{
		if (request.getReqTarget().compare(0, it->getPath().size(), it->getPath()) == 0
			&& it->getPath().size() > maxCoincidence)
		{
			locationIndex = res;
			maxCoincidence = it->getPath().size();
		}
	}
	if (locationIndex == -1)
		throw std::logic_error("location not found");
	return locations[locationIndex];
//	status = checkSource();
}

std::string checkSource(const Location& location, const std::string& reqTarget)
{
	std::string pathObj;
	if (reqTarget == "/")
		pathObj = location.getRoot() + location.getIndex();
	else
		pathObj = location.getRoot() + reqTarget.substr(1);

	std::ifstream file(pathObj);

	if (!file)
	{
		pathObj = getcwd(0x0, 0);
		pathObj += "/config/error_pages/404.html";
		throw std::logic_error("NOT_FOUND");
	}
	if (location.getAutoindex())
	{
		pathObj = getcwd(0x0, 0);
		pathObj += "/config/error_pages/403.html";
		throw std::logic_error("FORBIDDEN");
	}
	file.close();
	return (pathObj);
}

Response GetGenerator(const Request &request, const ServConfig &config) {
	Response rsp(200);
	const Location& location = getLocation(request, config);
	const std::string obj = checkSource(location, request.getReqTarget());

	std::ifstream file(obj);
	std::stringstream ss;
	ss << file.rdbuf();
	file.close();

	rsp.SetBody(ss.str());
	rsp.SetHeader("content-type", getMimeType(obj));
	return rsp;

}

Response generate_response(const Request& request, const ServConfig &config) {
	if(request.getMethod() == GET)
		return GetGenerator(request, config);
	return Response(500);
}

bool Client::response() {
	if (status_ != READY_TO_SEND)
		return false;
	Response rsp = generate_response(req_, serv_);
	std::string response = rsp.Generate();
	send(fd_, response.c_str(), response.length() + 1, 0); //TODO: CHECK RET
	status_ = READY_TO_READ;
	req_.clear();
	return status_ == CLOSE_CONNECTION;
}

const ServConfig &Client::getServ() const {
	return serv_;
}

int Client::getFd() const {
	return fd_;
}

void Client::receive() {
	if (status_ != READY_TO_READ)
		return;
	try {
		req_.receive();
		if(req_.isComplete()) {
			status_ = READY_TO_SEND;
			std::cout << "<REQUEST\n" << req_ << std::endl;
			std::cout << "REQUEST>\n"; //test
		}
	}
	catch (std::exception &) {
		std::cout << "TODO: handle the half msg!!!" << std::endl;
	}
}
__deprecated Client::Client(const ServConfig &serv, int fd) : serv_(serv), fd_(fd), status_(READY_TO_READ), req_(fd)
{}

Client::~Client() {
	close(fd_);
}

Client::Client(const ServConfig &serv, int fd, const sockaddr_in &clientAddr)
: serv_(serv), fd_(fd), status_(READY_TO_READ), req_(fd), _clientAddr(clientAddr)
{}

