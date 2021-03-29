#include <fstream>
#include <method_utils.h>
#include <mimeTypes.h>
#include "GetMethod.h"
#include "ServConfig.h"

Response GetGenerator(const Request &request, const ServConfig &config) {
	Response rsp(200);
	const Location &location = config.getLocation(request.getReqTarget());
	const std::string obj = checkSource(location, request.getReqTarget());
	//TODO::перенести общее для всех методов

//	std::ifstream file(obj);
//	std::stringstream ss;
//	ss << file.rdbuf();
//	file.close();
//	std::ifstream file(obj, std::ios::binary);
//	std::streambuf* raw_buffer = file.rdbuf();
//
//	char* block = new char[size];
//	raw_buffer->sgetn(block, size);
//	delete[] block;
//	rsp.SetBody(ss.str());


	FILE* f = fopen(obj.c_str(), "r");

	// Determine file size
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);

	std::string body;
	body.reserve(size);
	rewind(f);
	fread(const_cast<char *>(body.data()), sizeof(char), size, f);
	fclose(f);
	//tmp part
	{
		struct stat sb;
		lstat(obj.c_str(), &sb);
		char buff[1024];
		tm date;
		strptime(std::to_string(sb.st_mtimespec.tv_sec).c_str(), "%s", &date);
		size_t ret = strftime(buff, 1024, "%a, %d %b %Y %T GMT", &date);
		buff[ret] = '\0';
		rsp.SetHeader("Last-Modified", buff);

		size_t i;
		i =  body.find("<html lang=");
		rsp.SetHeader("Content-Language", i != std::string::npos ? body.substr(i + 12,  2) : "");
	}

	rsp.SetHeader("content-type", getMimeType(obj));
	return rsp;
}
