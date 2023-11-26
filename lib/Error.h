#pragma once
#include <iostream>
#include <string>

class Error
{
private:
	std::string errmsg;
public:
	Error();

	Error(std::string code, std::string msg, size_t line, size_t col);

	Error(Error const& rhs);

	Error& operator=(Error const& rhs);

	friend std::ostream& operator<<(std::ostream& os, Error& err);
};

