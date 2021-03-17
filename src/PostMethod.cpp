#include <method_utils.h>
#include <mimeTypes.h>
#include <fcntl.h>
#include "PostMethod.h"
#include "fstream"
#include "method_utils.h"


typedef std::map<std::string, std::string> EnvironMap;

std::string checkSource_ft(const Location &location, const std::string &reqTarget) {
	std::string pathObj;
	if (reqTarget   == "/") //TODO: WHY?!
		pathObj = location.getRoot() + location.getIndex();
	else
		pathObj = location.getRoot() + reqTarget.substr(1); // /user/www/folder
	std::ifstream file(pathObj);
	if (!file) {
		throw RespException(Response(404));
	}
	file.close();
	if (location.getAutoindex()) {
		throw RespException(Response(403));
	}
	return (pathObj);
}

std::map<std::string, std::string> CgiGenerateEnv()
{
	std::map<std::string, std::string> env;
	env["AUTH_TYPE"] = ""; //TODO:: ?
	env["CONTENT_LENGTH"] = ""; //TODO:: ?
	env["CONTENT_TYPE"] = ""; //TODO:: ?
	env["GATEWAY_INTERFACE"] = ""; //TODO:: ?
	env["PATH_INFO"] = ""; //TODO:: ?
	env["PATH_TRANSLATED"] = ""; //TODO:: ?
	env["QUERY_STRING"] = ""; //TODO:: ?
	env["REMOTE_ADDR"] = ""; //TODO:: ?
	env["REMOTE_IDENT"] = ""; //TODO:: ?
	env["REMOTE_USER"] = ""; //TODO:: ?
	env["REQUEST_METHOD"] = ""; //TODO:: ?
	env["REQUEST_URI"]  = ""; //getRequestUri(request.getReqTarget());
	env["SCRIPT_NAME"] = ""; //TODO:: ?
	env["SERVER_NAME"] = "";//getServerName();
	env["SERVER_PORT"] = ""; //TODO:: ?
	env["SERVER_PROTOCOL"] = ""; //TODO:: ?
	env["SERVER_SOFTWARE"] = ""; //TODO:: ?
	return env;
}

//char *Cgi::getServerName()
//{
//	return (strdup("SERVER_NAME="));
//}
//
//char *Cgi::getRequestUri(const std::string &uri)
//{
//	return (strdup(uri.c_str()));
//}

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

char ** map2envp(const EnvironMap& env_map)
{
	using std::string;
	using std::map;
	char** ret = new char*[env_map.size() + 1];
	ret[env_map.size()] = NULL;
	map<string, string>::const_iterator it = env_map.begin();
	size_t i = 0;
	for(; it != env_map.end(); it++, i++)
	{
		ret[i] = strdup((it->first + "=" + it->second).c_str());
	}
	return ret;
}

std::string CgiEx(const std::string& cgi, const std::string& script, const std::string input,
		   const EnvironMap & env_map)
{
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
		char * const *envp= map2envp(env_map);
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
		waitpid(pid, &ret, 0);
		while ((ret = read(Opipe.ReadFd(), buff, 1024)) > 0) {
			buff[ret] = '\0';
			total += buff; //TODO: Maybe i need only one
		}
		return total;
	}
}

std::string RunCgi(const Request &request, const ServConfig &config)
{
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
		Location location = getLocation(request.getReqTarget(), config);
		std::string cgi = location.getCgiPath(); //TODO: check cgi extension
		std::string script = checkSource_ft(location, request.getReqTarget());
		char * const args[3] = {strdup(cgi.c_str()), strdup(script.c_str()), NULL}; //TODO: LEAK!
		char * const * envp = {NULL};
		execve(cgi.c_str() , args, envp);
		exit(1);
	} else {
		close(Opipe.WriteFd());
		close(Ipipe.ReadFd());
		write(Ipipe.WriteFd(), request.getBody().c_str(), request.getBody().length());
		close(Ipipe.WriteFd());
		char buff[1025];
		int ret;
		std::string total;
		while ((ret = read(Opipe.ReadFd(), buff, 1024)) > 0) {
			buff[ret] = '\0';
			total += buff;
		}
		close(Opipe.ReadFd());
		waitpid(pid, &ret, 0);
		while ((ret = read(Opipe.ReadFd(), buff, 1024)) > 0) {
			buff[ret] = '\0';
			total += buff; //TODO: Maybe i need only one
		}
		return total;
		//TODO :: close FD change?!
	}
}

Response PostGenerator(const Request &request, const ServConfig &config) {
	Response rsp(201);
	Location location = getLocation(request.getReqTarget(), config);
	//std::string total = RunCgi(request, config);
	EnvironMap tmp;
	std::string total = CgiEx(location.getCgiPath(), request.getReqTarget(), request.getBody(), tmp);
	rsp.SetBody(total);
	//rsp.SetHeader("content-type", getMimeType(obj));
	return rsp;
}