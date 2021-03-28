#pragma once
#include "response.h"
#include "Request.h"
#include "ServConfig.h"

Response get_response(const Request &request, const ServConfig &config);
