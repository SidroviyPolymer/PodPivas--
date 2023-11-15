#pragma once
#include <iostream>
#include <string>

class Token {
public:
	enum Type {
		Id,
		Const,
		Terminal,
		Operation
	};

private:
	std::string content;
	Type type; 

public:
	Token();
	Token(std::string, Type);

	Type GetType();

	std::string& GetContent();

	friend std::ostream& operator<<(std::ostream&, const Token&);
};
