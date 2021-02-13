#pragma once
#include <libc.h>

class FD
{
private:
	FD();
	FD(FD &copy);
	const FD& operator=(const FD &rhs);

	int _fd;

public:
	FD(int fd);
	~FD();
	bool operator==(const FD &rhs) const;
	bool operator!=(const FD &rhs) const;
};