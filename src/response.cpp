#include "response.h"
#include "utils.h"
#include <vector>

Response::Response(int code) : code_(code) {}

void Response::SetDefaultContent() {
	if (code_ >= 400 && body_.empty())
		generate_error_page(code_);
	SetHeader("Date", GetCurDate());
	SetHeader("Server", "KingGinx");
	if (GetHeader("Content-length").empty())
		SetHeader("Content-Length", body_.length());
}

std::string Response::toString() const {
	std::ostringstream str_out;
	str_out << "HTTP/1.1 " << code_ << " " << g_resp_codes.at(code_) << "\r\n";
	for (Headers::const_iterator it = headers_.begin(); it != headers_.end(); it++)
		str_out << it->first << ": " << it->second << "\r\n";
	str_out << "\r\n";
	str_out << body_;
	return str_out.str();
}

void Response::SetHeader(const std::string &title, const std::string &content) {
	headers_[ft::tolower(title)] = content;
}

void Response::SetHeader(const std::string &title, size_t content) {
	headers_[ft::tolower(title)] = ft::to_str(content);
}

void Response::SetBody(const std::string &body) {
	body_ = body;
}

std::string Response::GetHeader(const std::string &title) const {
	try {
		return headers_.at(ft::tolower(title));
	} catch (const std::out_of_range &) {
		return "";
	}
}

std::string Response::GetCurDate() {
	timeval time;
	tm date;
	char buff[1024];

	gettimeofday(&time, NULL);
	strptime(std::to_string(time.tv_sec).c_str(), "%s", &date);
	size_t ret = strftime(buff, 1024, "%a, %d %b %Y %T GMT", &date);
	buff[ret] = '\0';
	return buff;
}

void Response::generate_error_page(int code) {
	std::ostringstream response_body;
	response_body << "<title> Error " << code << "</title>\r\n";
	response_body << "<h1>Code: " << code << "</h1>"
				  << "<h2>" << g_resp_codes.at(code_) << "</h2>\r\n"
				  << "<h1>" << "Сервера ответ!" << "</h2>\r\n"
				  << "<em><small>king's server</small></em>\r\n";
	SetBody(response_body.str());
	SetHeader("Content-Type", "text/html");
}

void Response::SetHeader(const std::string &title, const std::vector<std::string> &multi_content) {
	typedef std::vector<std::string>::const_iterator vectorIterator;

	std::string content;
	for (vectorIterator it = multi_content.begin(); it != multi_content.end(); it++) {
		if (it != multi_content.begin())
			content += ", ";
		content += *it;
	}
	SetHeader(title, content);
}

int Response::GetCode() const {
	return code_;
}
void Response::SetCode(int code) {
	code_ = code;
}
const Response &RespException::GetRsp() const {
	return rsp_;
}

RespException::RespException(const Response &rsp) throw(): rsp_(rsp) {}
const char *RespException::what() const throw() {
	return "HTTP ERRROR";
}
RespException::~RespException() throw() {}
