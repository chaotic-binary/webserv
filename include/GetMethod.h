#pragma once
#include "response.h"
#include "Request.h"
#include "ServConfig.h"
Response GetGenerator(const Request &request, const ServConfig &config);
