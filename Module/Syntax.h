#pragma once

#include "../lib/List.hpp"
#include "../lib/Token.h"
#include "../lib/Tree.h"
#include "../lib/Error.h"
#include "../lib/ID.h"

class Syntax {
private:
	void Program();
	bool DefineName(Token&, ID::Type, std::string);
	bool Semicolon(Token);

	bool Block(Tree*, std::string);

	bool ConstantSection(Tree*, std::string);
	bool DefinitionConstant(Tree*, std::string);
	bool ConstantExpression(Tree*);
	bool ConstantTerm(Tree*);
	bool ConstantFactor(Tree*);
	bool Constant(Token);

	bool VariableSection(Tree*, std::string);
	bool DescriptionSimilarVar(Tree*, std::string);

	Tree* syntaxTree;

	List<Token>* tokens;
	List<ID>* ids;
	List<Error>* errlist;

	bool isGood = true;
public:
	Syntax(List<Token>* tokens, List<ID>* ids, List<Error>* errlist);

	bool Process();

	Tree* GetTree();
};