#pragma once

#include <map>
#include <string>
#include <sstream>
#include <libc.h>

std::map<int, std::string> generate_map_codes();

const std::map<int, std::string> g_resp_codes = generate_map_codes();

class Response {
public:
	typedef std::map<std::string, std::string> Headers;

	Response(int code) : code_(code) {}

	std::string Generate() {
		if (code_ >= 400)
			generate_error_page(code_);
		SetHeader("Date", GetCurDate());
		SetHeader("Server",  "KingGinx");
		//TODO: Allow
		if (!body_.empty())
			SetHeader("Content-Length", std::to_string(body_.length()));
		std::ostringstream str_out;
		str_out << "HTTP/1.1 " << code_ << " " << g_resp_codes.at(code_) << "\r\n";
		for (Headers::iterator it = headers_.begin(); it != headers_.end(); it++)
			str_out << it->first << ": " <<  it->second << "\r\n";
		str_out << "\r\n";
		str_out << body_;
		return str_out.str();
	}

	void SetHeader(const std::string &title, const std::string &header) {
		headers_[title] = header;
	}

	void SetBody(const std::string& body) {
		body_ = body;
	}

	std::string GetHeader(const std::string &title) const {
		return headers_.at(title);
	}

private:
	std::string GetCurDate() {
		timeval time;
		tm date;
		char buff[1024];

		gettimeofday(&time, NULL);
		strptime(std::to_string(time.tv_sec).c_str(), "%s", &date);
		size_t ret = strftime(buff, 1024, "%a, %d %b %Y %T GMT", &date);
		buff[ret] = '\0';
		return buff;
	}

	void generate_error_page(int code) {
		std::ostringstream response_body;
		response_body << "<title> Error " << code << "</title>\r\n";
		response_body << "<h1>Code: " << code << "</h1>"
		<< "<h2>" << g_resp_codes.at(code_) << "</h2>\r\n"
		<< "<em><small>king's server</small></em>\r\n";
		SetBody(response_body.str());
		headers_["Content-Type"] = "text/html";
	}
	int code_;
	Headers headers_;
	std::string body_;
};