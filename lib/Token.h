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
	int line;
	int column;	
public:
	Token();
	Token(std::string, Type);
	Token(std::string, Type, int ,int);

	Type GetType();

	std::string& GetContent();

	std::pair<int, int> GetPos();

	friend std::ostream& operator<<(std::ostream&, const Token&);
};
