#pragma once
#include <string>

class Token {
public:
	enum Type {
		Id,
		Const,
		Terminal
	};

private:
	std::string content;
	Type type; 

public:
	Token();

	Type GetType();

	std::string& GetContent();
};
