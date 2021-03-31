#pragma once

#include "response.h"
#include "Request.h"
#include "ServConfig.h"

Response GetGenerator(const Request &request, const ServConfig &config);
Response PutGenerator(const Request &request, const ServConfig &config);
Response PostGenerator(const Request &request, const ServConfig &config);

typedef Response (*func)(const Request&, const ServConfig&);

const std::map<e_methods, func> init_method_map();

const std::map<e_methods, func> method_map = init_method_map();

inline const std::map<e_methods, func> init_method_map() {
	std::map<e_methods, func> mp;
	mp[GET] = GetGenerator;
	mp[HEAD] = GetGenerator;
	mp[PUT] = PutGenerator;
	mp[POST] = PostGenerator;
	return mp;
}
