#include <unistd.h>
#include <cerrno>
#include <utils.h>
#include <Request.h>
#include <ServConfig.h>
#include <method_utils.h>
#include <libc.h>
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
	//std::cerr << "PATH_INFO" << env["PATH_INFO"] << std::endl;
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

bool check_fd(int fd) {
	fd_set test;
	FD_ZERO(&test);
	FD_SET(fd, &test);
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	select(fd, &test, NULL, NULL, &tv);
	if (FD_ISSET(fd, &test))
		return true;
	else
		return false;
}
std::string CgiEx(const std::string &cgi,
				  const std::string &script,
				  const std::string &input,
				  const EnvironMap &env_map) {
	static const size_t BUFF_SIZE = 1024;
	char buff[BUFF_SIZE + 1];
	Pipe Opipe;
	Opipe.Create();
	std::string tmp_file_path(getcwd(buff, BUFF_SIZE)); //TODO: check ret
	tmp_file_path += "tmp_kinginx.tmp";
	int fd_bla = open(tmp_file_path.c_str(), O_TRUNC | O_CREAT | O_WRONLY, 0666);
	write(fd_bla, input.c_str(), input.size());
	close(fd_bla);
	fd_bla = open(tmp_file_path.c_str(), O_RDONLY); // TODO: OMG  This is piece of sh... code
	pid_t pid = fork();
	if (pid == -1)
		throw std::runtime_error("FORK ERROR");
	else if (pid == 0) {
		dup2(Opipe.WriteFd(), 1);
		dup2(fd_bla, 0);
		Opipe.Close();
		char *const args[3] = {strdup(cgi.c_str()), strdup(script.c_str()), NULL};
		char *const *envp = map2envp(env_map);
		execve(cgi.c_str(), args, envp);
		exit(1);
	} else {
		close(fd_bla);
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

		total.erase(0, total.find("\r\n\r\n") + 4); //TODO: PARSE! eto #kostil
		return total;
	}
}
