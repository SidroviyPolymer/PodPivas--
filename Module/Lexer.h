#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "../lib/List.hpp"
#include "../lib/Token.h"
#include "../lib/Error.h"
#include "../lib/ID.hpp"

class Pos {
private:
	int Line;
	int Column;
public:

	Pos() {

	}

	Pos(int Line, int Column) {
		this->Line = Line;
		this->Column = Column;
	}
	int& GetLine() {
		return Line;
	}
	int& GetColumn() {
		return Column;
	}

	//friend std::ostream& operator<<(std::ostream&, const Pos&);

	Pos& operator=(const Pos& pos) {

		if (this == &pos) {
			return *this;
		}

		Line = pos.Line;
		Column = pos.Column;
		return *this;
	}

	Pos(const Pos& p) {
		Line = p.Line;
		Column = p.Column;
	}
};


/*std::ostream& operator<<(std::ostream& info, const Pos& data) {
	info << "<" << data.Line << ", " << data.Column << ">";
	return info;
}*/

class Lexer {
private:
	std::ifstream is;

	typedef std::pair<int, int> posType;

	List<std::string>* flow;
	List<Pos>* flow2;

	List<Token>* tokens;
	List<std::string>* ids;

	const List<std::string> terminals = List<std::string>("program", "const", "var", "procedure", "integer", "begin", "end", "end.", ",", ":", ";", "(", ")");
	const List<std::string> operations = List<std::string>(":=", "+", "-", "*", "div", "mod");

	bool OpenFile(std::string src);
	void Parse();
	void TokenList(bool& result);

	void Id(std::string,Pos);
	void Terminal(std::string, Pos);
	void Operation(std::string, Pos);
	void Constant(std::string, Pos,bool& result);
public:
	Lexer();

	bool Process(List<Token>*, List<std::string>*, std::string src);
};