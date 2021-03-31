#include "response_generator.h"
#include <vector>
#include <string>
#include <ServConfig.h>
#include <methods.h>
#include <base64.h>
#include "response.h"
#include "utils.h"
#include "Request.h"


std::vector<std::string> translate_methods(const std::vector<e_methods> &allowed_methods) {
    std::vector<std::string> ans;

    for (size_t i = 0; i < allowed_methods.size(); i++)
        ans.push_back(ft::to_str(allowed_methods[i]));
    return ans;
}

bool autentificate(const Request &request, const Location &location) {
    if (location.getAuthorization().empty())
        return true;
    std::string authoriz = request.getHeader("Authorization");
    std::vector<std::string> tokens = ft::split(authoriz, ' ');
    if (tokens.size() != 2 || ft::tolower(tokens[0]) != "basic")
        return false;
    if (tokens[1] == base64_encode(location.getAuthorization()))
        return true;
    return false;
}

Response generate_response(const Request &request, const ServConfig &config) {
    if (!request.isComplete())
        return Response(400);
    try {
        const Location &location = config.getLocation(request.getReqTarget());
        const std::vector<e_methods> &allowedMethods = location.getMethods();
        if (request.getVersion() != "HTTP/1.1")
            return Response(505); //TODO: are lower versions supported?
        if (!autentificate(request, location)) {
            Response rsp(401);
            rsp.SetHeader("WWW-Authenticate", "Basic realm=\"Access to the staging site\", charset=\"UTF-8\"");
            return rsp;
        } //TODO:move down
        if (request.getBody().size() > location.getMaxBody())
            return Response(413);
        else if (!method_map.count(request.getMethod()))
            return Response(501);
        else if (allowedMethods.empty() ||
                 allowedMethods.end() == find(allowedMethods.begin(), allowedMethods.end(), request.getMethod())) {
            Response response(405);
            response.SetHeader("Allow", translate_methods(allowedMethods));
            return response;
        }
        if ((request.getMethod() == PUT || request.getMethod() == POST)
            && request.getHeader("content-length").empty() && !request.isChunked())
            return Response(411);
        return method_map.at(request.getMethod())(request, config);
    } catch (const RespException &err_rsp) {
        return err_rsp.GetRsp();
    } catch (...) {
        return Response(500);
    }
}


Response get_response(const Request &request, const ServConfig &config) {
    Response response = generate_response(request, config);
    response.SetDefaultContent(config.getErrorPages());
    if (request.getMethod() == HEAD)
        response.SetBody("");
    return response;
}