#include "method_utils.h"
#include <algorithm>
#include "response.h"
#include "Cgi.h"


Response PostGenerator(const Request &request, const ServConfig &config) {
	Response rsp(200);
	return CgiResponse(request, config);
}