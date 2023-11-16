#pragma once
#include <string>
using namespace std;

class ID {
private:
	int id;
	string core;
public:
	ID() {};
	ID(int id, string core) {
		this->id = id;
		this->core = core;
	};
	ID(std::string core) {
		this->core = core;
	};
	~ID() {};
};