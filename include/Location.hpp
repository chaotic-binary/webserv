#ifndef WEBSERV_LOCATION_HPP
#define WEBSERV_LOCATION_HPP

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
	TRACE
};

class Location
{
public:
	Location();
	Location(const Location &copy);
	~Location();
	Location &operator=(const Location &copy);

	void setName(const std::string &name);
	void setRoot(const std::string &root);
	void setIndex(const std::string &index);
	void setCgiPath(const std::string &cgiPath);
	void setUploadPath(const std::string &uploadPath);
	void setMaxBody(size_t maxBody);
	void setAutoindex(bool autoindex);
	void setUploadEnable(bool uploadEnable);
	void setCgiExtensions(const std::vector<std::string> &cgiExtensions);
	void setMethods(const std::vector<e_methods> &methods);

	void setMethodsFromStr(const std::vector<std::string> &methods);
	void setAutoindexFromStr(const std::string &str);
	void setUploadEnableFromStr(const std::string &str);

	const std::string &getName() const;
	const std::string &getRoot() const;
	const std::string &getIndex() const;
	const std::string &getCgiPath() const;
	const std::string &getUploadPath() const;
	size_t getMaxBody() const;
	bool getAutoindex() const;
	bool getUploadEnable() const;
	const std::vector<std::string> &getCgiExtensions() const;
	const std::vector<e_methods> &getMethods() const;
	static const std::map<std::string, e_methods> &getMethodsParser();

	static void initMethodsParser();

	class LocException: public std::exception {
	public:
		class WrongMethod: public std::exception {
			virtual const char* what() const throw();
		};
		class WrongOnOff: public std::exception {
			virtual const char* what() const throw();
		};
		//virtual const char* what() const throw();
	};

	friend std::ostream &operator<<(std::ostream &os, const Location &location);
	friend std::ostream &operator<<(std::ostream &os, const std::vector<e_methods> &v);

private:
	std::string					_name;
	std::string					_root;
	std::string					_index;
	std::string					_cgiPath;
	std::string					_uploadPath;
	size_t						_maxBody;
	bool						_autoindex;
	bool						_uploadEnable;
	std::vector<std::string>	_cgiExtensions;
	std::vector<e_methods>		_methods;

	static bool getBoolFromStr(const std::string &str);
	static std::map< std::string, e_methods> methodsParser;
};

#endif //WEBSERV_LOCATION_HPP
