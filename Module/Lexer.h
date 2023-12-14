#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "../lib/List.hpp"
#include "../lib/Token.h"
#include "../lib/Error.h"
#include "../lib/ID.h"

class Pos {
private:
	int Line;
	int Column;	
public:

	Pos();

	Pos(int Line, int Column);

	int& GetLine();
	int& GetColumn();

	Pos& operator=(const Pos& pos);

	Pos(const Pos& p);

	friend std::ostream& operator<<(std::ostream&, Pos const&);
};


class Lexer {
private:
	std::ifstream is;
	List<Error>* errlist;

	typedef std::pair<int, int> posType;

	List<std::string>* flow;
	List<Pos>* flow2;

	List<Token>* tokens;
	List<ID>* ids;

	const List<std::string> terminals = List<std::string>("program", "const", "var", "procedure", "integer", "begin", "end", ".", ",", ":", ";", "(", ")","exit"); // Терминальные символы
	const List<std::string> TerminalsForID = List<std::string>("const", "var", "procedure", "begin", "end", "end.");
	const List<std::string> operations = List<std::string>(":=", "+", "-", "*", "div", "mod", "="); // Операторы

	bool OpenFile(std::string src);
	void Parse();
	void TokenList(bool& result);
	void IDList(Pos, List<Token>*);

	void Id(std::string,Pos);
	void Terminal(std::string, Pos);
	void Operation(std::string, Pos);
	void Constant(std::string, Pos,bool& result);
public:
	Lexer();

	bool Process(List<Token>*, List<ID>*, std::string src, List<Error>* errlist);
};