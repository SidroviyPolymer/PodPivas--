#pragma once

#include "../lib/List.hpp"
#include "../lib/Token.h"
#include "../lib/Tree.hpp"
#include "../lib/Error.h"
#include "../lib/ID.h";

class Syntax {
private:
	void Program();
	bool DefineName(Token&, ID::Type, std::string);
	bool Semicolon(Token);

	bool Block(Tree*, std::string);

	bool ConstantSection(Tree*, std::string);
	bool DefinitionConstant(Tree*, std::string);
	bool Constant(Token);

	Tree* syntaxTree;

	List<Token>* tokens;
	List<ID>* ids;
	List<Error>* errlist;

	bool isGood;
public:
	Syntax(List<Token>* tokens, List<ID>* ids, List<Error>* errlist);

	bool Process();
};