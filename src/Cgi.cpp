#include <unistd.h>
#include <cerrno>
#include <utils.h>
#include <Request.h>
#include <ServConfig.h>
#include <cmath>
#include <libc.h>
#include <response.h>
#include "Cgi.h"

struct Pipe {
    Pipe() : created(false), closed(false) {}

    void Close() {
        if (created && !closed) {
            close(pipefd[0]);
            close(pipefd[1]);
            closed = true;
        }
    }

    int ReadFd() const {
        return pipefd[0];
    }

    void Create() {
        if (created && closed)
            return;
        if (pipe(pipefd) < 0)
            throw std::runtime_error(strerror(errno));
        created = true;
    }

    int WriteFd() const {
        return pipefd[1];
    }

    ~Pipe() {
        Close();
    }

private:
    int pipefd[2];
    bool created;
    bool closed;
};

static std::string getHttpEnv(std::string title) {
    title = ft::toupper(title);
    for (size_t index = 0; index < title.size(); index++)
        if (title[index] == '-')
            title[index] = '_';
    return "HTTP_" + title;
}

std::map<std::string, std::string> CgiGenerateEnv(const Request &request, const ServConfig &config, const std::string& path) {
    std::map<std::string, std::string> env;
    env["CONTENT_LENGTH"] = std::to_string(request.getBody().size());
    env["CONTENT_TYPE"] = request.getHeader("content-type");
    env["QUERY_STRING"] = request.GetQueryString();
    env["REQUEST_METHOD"] = ft::to_str(request.getMethod());
    env["SERVER_PORT"] = std::to_string(config.getPort());
    env["SERVER_PROTOCOL"] = "HTTP/1.1";
    env["SERVER_SOFTWARE"] = "KinGinx/0.42";
    env["GATEWAY_INTERFACE"] = "CGI/1.1";
    env["SERVER_NAME"] = request.getHeader("host");
    env["SCRIPT_NAME"] = path;
	env["SCRIPT_FILENAME"] = path; //FOR PHP_CGI;
    env["REQUEST_URI"] = request.GetUri();

    std::string authType = request.getHeader("Authorization");
    size_t found = authType.find(" ");
    if (found != std::string::npos)
        authType = authType.substr(0, found);
    env["AUTH_TYPE"] = authType;
    env["PATH_INFO"] = request.getReqTarget();
    env["PATH_TRANSLATED"] = env["PATH_INFO"];
    env["REDIRECT_STATUS"] = "YA NAGNUL PHP_CGI";
    std::map<std::string, std::string>::const_iterator it = request.getHeaders().begin();
    for (; it != request.getHeaders().end(); it++)
        env[getHttpEnv(it->first)] = it->second;
    return env;
}

static char **map2envp(const EnvironMap &env_map) {
    using std::string;
    using std::map;
    char **ret = new char *[env_map.size() + 1];
    ret[env_map.size()] = NULL;
    map<string, string>::const_iterator it = env_map.begin();
    size_t i = 0;
    for (; it != env_map.end(); it++, i++)
        ret[i] = strdup((it->first + "=" + it->second).c_str());
    return ret;
}

std::string CgiEx(const std::string &cgi,
				  const std::string &script,
				  const std::string &input,
				  const EnvironMap &env_map) {
	static const size_t BUFF_SIZE = pow(2, 20);
	char buff[BUFF_SIZE + 1];
	const std::string tmp_file_path = getcwd(buff, BUFF_SIZE) + std::string("/.buffer_file_kinGinx.tmp");
	int fileBufferFd = open(tmp_file_path.c_str(), O_TRUNC | O_CREAT | O_WRONLY, 0666);
	write(fileBufferFd, input.c_str(), input.size());
	close(fileBufferFd);
	fileBufferFd = open(tmp_file_path.c_str(), O_RDONLY);

    Pipe Opipe;
    Opipe.Create();

    pid_t pid = fork();
    if (pid == -1)
        throw std::runtime_error("FORK ERROR");
    else if (pid == 0) {
        dup2(Opipe.WriteFd(), 1);
        dup2(fileBufferFd, 0);
        close(fileBufferFd);
        Opipe.Close();
        char *const args[3] = {strdup(cgi.c_str()), strdup(script.c_str()), NULL};
        char *const *envp = map2envp(env_map);
        execve(cgi.c_str(), args, envp);
        exit(1);
    } else {
        close(fileBufferFd);
        close(Opipe.WriteFd());
        int ret;
        std::string total;
        while ((ret = read(Opipe.ReadFd(), buff, BUFF_SIZE)) > 0) {
            buff[ret] = '\0';
            total += buff;
        }
        close(Opipe.ReadFd());
        waitpid(pid, &ret, 0);
        if (WEXITSTATUS(ret) != 0)
            throw std::runtime_error("execve error");
        while ((ret = read(Opipe.ReadFd(), buff, BUFF_SIZE)) > 0) {
            buff[ret] = '\0';
            total += buff;
        }
        return total;
    }
}


static std::map<std::string, std::string> parse_headers(const std::string &str) {
    std::vector<std::string> v;
    std::vector<std::string> lines = ft::split(str, '\n');
    std::map<std::string, std::string> headers;
    size_t newPos;

    for (size_t i = 0; i < lines.size(); ++i) {
        if ((newPos = lines[i].find_first_of(':')) == std::string::npos)
            throw std::runtime_error("Invalid format");
        std::string tmp = lines[i].substr(0, newPos);
        tmp = ft::tolower(tmp);
        headers[tmp] = lines[i].substr(newPos + 2, lines[i].size() - 3 - newPos);
    }
    return headers;
}

void SetResponse(Response &response, const std::string &cgi_raw_response) {
    const static std::string EO_HEADERS = "\r\n\r\n";
    typedef std::map<std::string, std::string> Headers;
    size_t eo_headers_position = cgi_raw_response.find(EO_HEADERS);

    if (eo_headers_position == std::string::npos) {
        response.SetBody(cgi_raw_response);
        return;
    }

    Headers headers = parse_headers(cgi_raw_response.substr(0, eo_headers_position + 2));

    Headers::const_iterator it;
    for (it = headers.begin(); it != headers.end(); ++it) {
        if (it->first != "status")
            response.SetHeader(it->first, it->second);
        else
            response.SetCode(ft::to_num(headers["status"]));
    }
    response.SetBody(cgi_raw_response.substr(eo_headers_position + 4));
}


std::string getTargetPath(const Location& location, const std::string& request_target)
{
    std::string path = request_target;
    path.erase(0, location.getPath().size());
    if(path.size() != 0 && path.front() == '/') {
    	path.erase(path.begin());
    }
    path = location.getRoot() + path;

    struct stat sb;
    stat(path.c_str(), &sb);
    if ((S_ISDIR(sb.st_mode))) {
        path += (path.back() != '/') ? "/" : "";
        path += location.getCgiIndex();
    }
    const std::vector<std::string> &cgi_extensions = location.getCgiExtensions();
    size_t dot = path.find('.');
    if(dot == std::string::npos)
        throw Response(404);
    std::string extension = path.substr(dot);
    if(find(cgi_extensions.begin(), cgi_extensions.end(),extension) == cgi_extensions.end())
        throw Response(404);
    std::cerr << path << std::endl;
    return path;
}

Response CgiResponse(const Request &request, const ServConfig &config) {
    Response rsp(200);
    const Location &location = config.getLocation(request.getReqTarget());

    std::string path = getTargetPath(location, request.getReqTarget());
    EnvironMap tmp = CgiGenerateEnv(request, config, path);
    std::string cgi_raw_response = CgiEx(location.getCgiPath(), path, request.getBody(), tmp);
    SetResponse(rsp, cgi_raw_response);
    return rsp;
}
