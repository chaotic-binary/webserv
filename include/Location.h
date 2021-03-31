#pragma once

# include <iostream>
# include <vector>
# include <map>

#define DEF_MAX_BODY 1048576

enum e_methods {
  GET,
  HEAD,
  POST,
  PUT,
  DELETE,
  CONNECT,
  OPTIONS,
  TRACE,
  OTHER
};

class Location {
 public:
  Location();
  Location(const Location &copy);
  ~Location();

  void setPath(const std::string &path);
  void updatePath(const std::string &path);
  void setRoot(const std::string &root);
  void setIndex(const std::string &index);
  void setCgiIndex(const std::string &index);
  void setCgiPath(const std::string &cgiPath);
  void setUploadPath(const std::string &uploadPath);
  void setMaxBody(size_t maxBody);
  void setAutoindex(bool autoindex);
  void setUploadEnable(bool uploadEnable);
  void setCgiExtensions(const std::vector<std::string> &cgiExtensions);
  void setMethods(const std::vector<e_methods> &methods);
  void setAuthorization(const std::string& authorization);
  void setMethodsFromStr(const std::vector<std::string> &methods);
  void setAutoindexFromStr(const std::string &str);
  void setUploadEnableFromStr(const std::string &str);

  void updateRoot(const std::string &root_prefix);

  const std::string &getName() const;
  const std::string &getRoot() const;
  const std::string &getIndex() const;
  const std::string &getCgiIndex() const;
  const std::string &getCgiPath() const;
  const std::string &getUploadPath() const;
  const std::string &getPath() const;
  const std::string &getPathR() const;
  const std::string &getAuthorization() const;
  size_t getMaxBody() const;
  bool getAutoindex() const;
  bool getUploadEnable() const;
  const std::vector<std::string> &getCgiExtensions() const;
  const std::vector<e_methods> &getMethods() const;

  static const std::vector<std::string> &getMethodsParser();

  static void initMethodsParser();

  class DuplicateDirective : public std::logic_error {
   private:
	DuplicateDirective();

   public:
	explicit DuplicateDirective(const std::string &name) : std::logic_error("Duplicate directive: " + name) {};
  };

 private:
  std::string _root;
  std::string _index;
  std::string _cgiPath;
  std::string _uploadPath;
  size_t _maxBody;
  bool _autoindex;
  bool _uploadEnable;
  std::vector<std::string> _cgiExtensions;
  std::vector<e_methods> _methods;
  std::string _cgi_index;
  std::string _path;
  std::string _authorization;
  std::string _pathR;

  static bool getBoolFromStr(const std::string &str);
  static std::vector<std::string> methodsParser;
  int _parsed;
};

std::ostream &operator<<(std::ostream &os, const Location &location);

std::ostream &operator<<(std::ostream &os, const std::vector<e_methods> &v);
