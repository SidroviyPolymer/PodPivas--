#pragma once
#include <iostream>
#include <string>

class ID {
public:
	enum Type {
		Var,
		Const,
		Proc,
		Not_defined
	};

private:
	std::string content;
	Type type;
	int Line;
	int Column;
public:
	ID();
	ID(std::string, Type);
	ID(std::string, Type, int, int);

	Type GetType();

	std::string& GetContent();

	friend std::ostream& operator<<(std::ostream&, const ID&);
	bool operator==(ID const&);
};