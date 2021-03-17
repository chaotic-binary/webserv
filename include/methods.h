#pragma once

#include "GetMethod.h"
#include "PostMethod.h"

#include <map>
typedef Response (*func)(const Request&, const ServConfig&);

const std::map<e_methods, func> init_method_map();

const std::map<e_methods, func> method_map = init_method_map();
inline const std::map<e_methods, func> init_method_map() {
	std::map<e_methods, func> mp;
	mp[GET] = GetGenerator;
	mp[POST] = PostGenerator;
	return mp;
}
