#include "FD.h"

FD::~FD() { close(_fd); }

bool FD::operator==(const FD &rhs) const { return _fd == rhs._fd; }

bool FD::operator!=(const FD &rhs) const { return !(rhs == *this); }
