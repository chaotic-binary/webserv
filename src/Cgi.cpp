#include <unistd.h>
#include <cerrno>
#include <utils.h>
#include <Request.h>
#include <ServConfig.h>
#include <method_utils.h>
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
	  if(created && closed)
		  return;
	  if(pipe(pipefd) < 0)
		  throw std::runtime_error(strerror(errno));
	  created = true;
  }
  int WriteFd() const {
	  return pipefd[1];
  }
  ~Pipe() {
	  Close(); }
 private:
  int pipefd[2];
  bool created;
  bool closed;
};

static std::string getHttpEnv(std::string title)
{
	title = ft::to_upper(title);
	for (size_t index = 0; index < title.size(); index++)
		if(title[index] == '-')
			title[index] = '_';
	return "HTTP_" + title;
}

std::map<std::string, std::string> CgiGenerateEnv(const Request &request, const ServConfig &config) {
	std::map<std::string, std::string> env;
	env["CONTENT_LENGTH"] = request.getHeader("content-length");
	env["CONTENT_TYPE"] = request.getHeader("content-type");
	env["QUERY_STRING"] = request.GetQueryString();
	env["REQUEST_METHOD"] = ft::to_str(request.getMethod());
	env["SERVER_PORT"] = std::to_string(config.getPort());
	env["SERVER_PROTOCOL"] = "HTTP/1.1";
	env["SERVER_SOFTWARE"] = "KinGinx/0.42"; //TODO:: Do we need system (Windows/Macos)...
	env["GATEWAY_INTERFACE"] = "CGI/1.1";
	env["SERVER_NAME"] = request.getHeader("host"); //TODO: look at nginx?!
	env["SCRIPT_NAME"] = request.getReqTarget(); //TODO:: ?
	env["REQUEST_URI"]  = request.GetUri();

	std::string authType = request.getHeader("Authorization");
	size_t found = authType.find(" ");
	if (found != std::string::npos)
		authType = authType.substr(0, found);
	env["AUTH_TYPE"] = authType;

	env["PATH_INFO"] = checkSource(config.getLocation(request.getReqTarget()), request.getReqTarget()); //TODO::TEST IT
	env["PATH_TRANSLATED"] = env["PATH_INFO"]; //TODO:: ?
	env["REMOTE_ADDR"] = ""; //TODO:: ?
	env["REMOTE_IDENT"] = ""; //TODO:: ?
	env["REMOTE_USER"] = ""; //TODO:: ?
	std::map<std::string, std::string>::const_iterator it = request.getHeaders().begin();
	for(; it != request.getHeaders().end(); it++)
		env[getHttpEnv(it->first)] = it->second;
	return env;
}




static char ** map2envp(const EnvironMap& env_map)
{
	using std::string;
	using std::map;
	char** ret = new char*[env_map.size() + 1];
	ret[env_map.size()] = NULL;
	map<string, string>::const_iterator it = env_map.begin();
	size_t i = 0;
	for(; it != env_map.end(); it++, i++)
		ret[i] = strdup((it->first + "=" + it->second).c_str());
	return ret;
}


std::string CgiEx(const std::string &cgi, const std::string &script, const std::string input, const EnvironMap &env_map) {
	std::cerr << script;
	Pipe Opipe, Ipipe;
	Opipe.Create();
	Ipipe.Create();
	pid_t pid = fork();
	if(pid == -1)
		throw std::runtime_error("FORK ERROR");
	else if (pid == 0) {
		dup2(Ipipe.ReadFd(), 0);
		Ipipe.Close();
		dup2(Opipe.WriteFd(), 1);
		Opipe.Close();
		char * const args[3] = {strdup(cgi.c_str()), strdup(script.c_str()), NULL};
		char * const * envp= map2envp(env_map);
		execve(cgi.c_str(), args, envp);
		exit(1);
	} else {
		close(Opipe.WriteFd());
		close(Ipipe.ReadFd());
		write(Ipipe.WriteFd(), input.c_str(), input.length());
		close(Ipipe.WriteFd());
		char buff[1025];
		int ret;
		std::string total;
		while ((ret = read(Opipe.ReadFd(), buff, 1024)) > 0) {
			buff[ret] = '\0';
			total += buff;
		}
		close(Opipe.ReadFd());
		waitpid(pid, &ret, 0); //TODO: Check ret
		if(WEXITSTATUS(ret) != 0)
			throw std::runtime_error("execve error");
		while ((ret = read(Opipe.ReadFd(), buff, 1024)) > 0) {
			buff[ret] = '\0';
			total += buff; //TODO: Maybe i need only one
		}
		return total;
	}
}
