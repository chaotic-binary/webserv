#pragma once

#include "ServConfig.h"
#include "Request.h"
#include "response.h"

Response PostGenerator(const Request &request, const ServConfig &config);
