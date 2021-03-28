#include <unistd.h>
#include <cerrno>
#include <utils.h>
#include <Request.h>
#include <ServConfig.h>
#include <method_utils.h>
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

std::map<std::string, std::string> CgiGenerateEnv(const Request &request, const ServConfig &config) {
	std::map<std::string, std::string> env;
	env["CONTENT_LENGTH"] = std::to_string(request.getBody().size());
	env["CONTENT_TYPE"] = request.getHeader("content-type");
	env["QUERY_STRING"] = request.GetQueryString();
	env["REQUEST_METHOD"] = ft::to_str(request.getMethod());
	env["SERVER_PORT"] = std::to_string(config.getPort());
	env["SERVER_PROTOCOL"] = "HTTP/1.1";
	env["SERVER_SOFTWARE"] = "KinGinx/0.42"; //TODO:: Do we need system (Windows/Macos)...
	env["GATEWAY_INTERFACE"] = "CGI/1.1";
	env["SERVER_NAME"] = request.getHeader("host"); //TODO: look at nginx?!
	env["SCRIPT_NAME"] = request.getReqTarget(); //TODO:: ????!!!!
	env["REQUEST_URI"] = request.GetUri();

	std::string authType = request.getHeader("Authorization");
	size_t found = authType.find(" ");
	if (found != std::string::npos)
		authType = authType.substr(0, found);
	env["AUTH_TYPE"] = authType;

	env["PATH_INFO"] = request.getReqTarget();
	env["PATH_TRANSLATED"] = env["PATH_INFO"]; //TODO:: ?
	env["REMOTE_ADDR"] = ""; //TODO:: ?
	env["REMOTE_IDENT"] = ""; //TODO:: ?
	env["REMOTE_USER"] = ""; //TODO:: ?
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

void partial_write(int fd, const std::string &msg) {
	size_t sended = 0;
	while (sended != msg.size()) {
		size_t send_len = msg.size() - sended < 10024 ? msg.size() - sended : 10024;
		sended += write(fd, &(msg.c_str()[sended]), send_len); //TODO: check write ret
	}
}

std::string CgiEx(const std::string &cgi,
				  const std::string &script,
				  const std::string &input,
				  const EnvironMap &env_map) {
	static const size_t BUFF_SIZE = 1024;
	char buff[BUFF_SIZE + 1];
	const std::string tmp_file_path = getcwd(buff, BUFF_SIZE) + std::string("/.buffer_file_kinGinx.tmp");
	int fileBufferFd = open(tmp_file_path.c_str(), O_TRUNC | O_CREAT | O_WRONLY, 0666);
	write(fileBufferFd, input.c_str(), input.size());
	close(fileBufferFd);
	fileBufferFd = open(tmp_file_path.c_str(), O_RDONLY); // TODO: OMG  This is piece of sh... code

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
			total += buff; //TODO: Maybe i need only one
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
			throw std::runtime_error("Invalid format"); //TODO:where we catch it?
		std::string tmp = lines[i].substr(0, newPos);
		tmp = ft::tolower(tmp);
		/*if (headers.count(tmp)) {
			if ((tmp == "host" || tmp == "content-length"))
				throw DuplicateHeader(tmp);
		}*/
		headers[tmp] = lines[i].substr(newPos + 2, lines[i].size() - 3 - newPos);
	}
	return headers;
}


Response CgiResponse(const Request &request, const ServConfig &config) {
	Response rsp(200);
	const Location &location = config.getLocation(request.getReqTarget());
	//TODO: is this cgi Correct?!
	//std::string path = checkSource(location, request.getReqTarget(), true);
	//std::cerr << "!!!!"  << path ;
	__unused    const std::vector<std::string> &cgi_extentions = location.getCgiExtensions();

//	if (cgi_extentions.empty() ||
//	cgi_extentions.end() == find(cgi_extentions.begin(), cgi_extentions.end(), path.substr(path.rfind('.'))))
//		return Response(404); //TODO do we need this checkout?
	std::string path = request.getReqTarget();
	path.erase(0, location.getPath().size() + 1);
	path = location.getRoot() + path;
	EnvironMap tmp = CgiGenerateEnv(request, config);
	std::string total = CgiEx(location.getCgiPath(), path, request.getBody(), tmp);
	//std::cout << total << location.getCgiPath() << std::endl;
	size_t i = total.find("\r\n\r\n");

		std::map<std::string, std::string> headers = parse_headers(total.substr(0, i + 2));
		//if (!headers.count("status"))
		//	throw TODO:?
		rsp.SetCode(ft::to_num(headers["status"]));
		std::map<std::string, std::string>::const_iterator it;
		for (it = headers.begin(); it != headers.end(); ++it) {
			if (it->first != "status")
				rsp.SetHeader(it->first, it->second);
		}
		total.erase(0, i + 4);
	rsp.SetBody(total);
	return rsp;
}
