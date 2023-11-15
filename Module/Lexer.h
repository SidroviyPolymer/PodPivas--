#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "../lib/List.hpp"
#include "../lib/Token.h"

class Lexer {
private:
	std::ifstream is;

	List<std::string>* flow;

	List<Token>* tokens;
	List<std::string>* ids;

	const List<std::string> terminals = List<std::string>("program", "const", "var", "procedure", "integer", "begin", "end", "end.", ",", ":", ";", "(", ")");
	const List<std::string> operations = List<std::string>(":=", "+", "-", "*", "div", "mod");

	bool OpenFile(std::string src);
	void Parse();
	void TokenList();

	void Id(std::string);
	void Terminal(std::string);
	void Operation(std::string);
	void Constant(std::string);
public:
	Lexer();

	bool Process(List<Token>*, List<std::string>*, std::string src);
};