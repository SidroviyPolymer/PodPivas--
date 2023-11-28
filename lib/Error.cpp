#include "Error.h"

Error::Error() {
	errmsg = "";
}

Error::Error(std::string code, std::string msg, size_t line, size_t col) {
	errmsg = code + ":\t" + msg + ",\tPos:\t(" + std::to_string(line) + ", " + std::to_string(col) + ")";
}

Error::Error(Error const& rhs) {
	errmsg = rhs.errmsg;
}

Error& Error::operator=(Error const& rhs) {
	if (this == &rhs)
		return *this;

	errmsg = rhs.errmsg;
	return *this;
}

std::ostream& operator<<(std::ostream& os, Error& err) {
	os << err.errmsg;
	return os;
}