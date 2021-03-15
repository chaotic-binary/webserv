#include "response.h"

std::map<int, std::string> generate_map_codes() {
	std::map<int, std::string> result;
	//1×× Informational
	result[100] = "Continue";
	result[101] = "Switching Protocols";
	result[102] = "Processing";
	//2×× Success
	result[200] = "OK";
	result[201] = "Created";
	result[202] = "Accepted";
	result[203] = "Non-authoritative Information";
	result[204] = "No Content";
	result[205] = "Reset Content";
	result[206] = "Partial Content";
	result[207] = "Multi-Status";
	result[208] = "Already Reported";
	result[226] = "IM Used";
	//3×× Redirection
	result[300] = "Multiple Choices";
	result[301] = "Moved Permanently";
	result[302] = "Found";
	result[303] = "See Other";
	result[304] = "Not Modified";
	result[305] = "Use Proxy";
	result[307] = "Temporary Redirect";
	result[308] = "Permanent Redirect";
	//4×× Client Error
	result[400] =  "Bad Reques";
	result[401] =  "Unauthorized";
	result[402] =  "Payment Required";
	result[403] =  "Forbidden";
	result[404] =  "Not Found";
	result[405] =  "Method Not Allowed";
	result[406] =  "Not Acceptable";
	result[407] =  "Proxy Authentication Required";
	result[408] =  "Request Timeout";
	result[409] =  "Conflict";
	result[410] =  "Gone";
	result[411] =  "Length Required";
	result[412] =  "Precondition Failed";
	result[413] =  "Payload Too Large";
	result[414] =  "Request-URI Too Long";
	result[415] =  "Unsupported Media Type";
	result[416] =  "Requested Range Not Satisfiable";
	result[417] =  "Expectation Failed";
	result[418] =  "I'm a teapot";
	result[421] =  "Misdirected Request";
	result[422] =  "Unprocessable Entity";
	result[423] =  "Locked";
	result[424] =  "Failed Dependency";
	result[426] =  "Upgrade Required";
	result[428] =  "Precondition Required";
	result[429] =  "Too Many Requests";
	result[431] =  "Request Header Fields Too Large";
	result[444] =  "Connection Closed Without Response";
	result[451] =  "Unavailable For Legal Reasons";
	result[499] =  "Client Closed Request";
	//5×× Server Error
	result[500] =  "Internal Server Error";
	result[501] =  "Not Implemented";
	result[502] =  "Bad Gateway";
	result[503] =  "Service Unavailable";
	result[504] =  "Gateway Timeout";
	result[505] =  "HTTP Version Not Supported";
	result[506] =  "Variant Also Negotiates";
	result[507] =  "Insufficient Storage";
	result[508] =  "Loop Detected";
	result[510] =  "Not Extended";
	result[511] =  "Network Authentication Required";
	result[599] =  "Network Connect Timeout Error";
	return result;
}
