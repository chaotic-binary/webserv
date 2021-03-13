#pragma once

# include <libc.h>
//# include <iostream>
# include <sstream>
#include <unistd.h>

class FD : public std::stringstream {
private:
	FD();
	FD(FD &copy);
	const FD& operator=(const FD &rhs);

	int _fd;

public:
	FD(int fd)
	{
		char buf[1024];
		std::string tmp;
		int ret;
		while((ret = ::read(fd, buf, 1024)))
		{
			buf[ret] = '\0';
			tmp += buf;
		}
		//this->re
		//TODO::???
	}

	virtual ~FD();
	bool operator==(const FD &rhs) const;
	bool operator!=(const FD &rhs) const;
};