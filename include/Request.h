#pragma once

# include "utils.h"
# include "Location.h"
# include <ostream>
# include <unistd.h>

enum e_client_status {
  CLOSE_CONNECTION, READY_TO_READ, READY_TO_SEND, SENDING
};

class Request {
 private:
  e_methods method;
  std::string reqTarget;
  std::string uri_;
  std::string queryString;
  std::string version;
  std::map<std::string, std::string> headers;
  std::string body;
  std::string raw_request;
  size_t contentLength;
  bool chunked;
  bool headersParsed;
  bool complete;
  const int fd_;

  void setMethodFromStr(const std::string &);

  void parse_headers(const std::string &str);
  int parse_body(const int fd, bool read_activated);
  int parse_chunk(const int fd, bool &read_activated);

  Request();

 public:
  Request(const int fd);
  virtual ~Request();

  e_methods getMethod() const;
  const std::string &getReqTarget() const;
  const std::string &getVersion() const;
  const std::map<std::string, std::string> &getHeaders() const;
  const std::string &getHeader(const std::string &title) const;
  const std::string &GetUri() const;
  const std::string &GetQueryString() const;
  const std::string &getBody() const;
  bool isComplete() const;

  class InvalidFormat : public std::logic_error {
   private:
	InvalidFormat();

   public:
	InvalidFormat(int line) : std::logic_error("Invalid data format: line:" + ft::to_str(line)) {};
  };

  class DuplicateHeader : public std::logic_error {
   private:
	DuplicateHeader();

   public:
	DuplicateHeader(std::string name) : std::logic_error("Duplicate header: " + name) {};
  };

  class HeaderNotPresent : public std::logic_error {
   private:
	HeaderNotPresent();

   public:
	HeaderNotPresent(std::string name) : std::logic_error("Header \"" + name + "\" not present") {};
  };

  int receive();

  void clear();
};

std::ostream &operator<<(std::ostream &os, const Request &request);


