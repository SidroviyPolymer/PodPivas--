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
	bool ConstantExpression(Tree*, int&);
	bool GetConstPostfix(List<Token>*);
	bool Constant(Token);

	bool VariableSection(Tree*, std::string);
	bool DescriptionSimilarVar(Tree*, std::string, size_t);

	bool OperatorsSection(Tree*, std::string, size_t);
	bool CompoundOperator(Tree*, std::string, size_t);
	bool Operator(Tree*, std::string, size_t);

	bool SimpleOperator(Tree*, std::string, size_t);
	bool AssigmentOperator(Tree*, std::string, size_t);
	bool Expression(Tree*);
	bool GetPostfix(List<Token>*);
	bool GetExpressionTree(Tree*, List<Token>*);
	bool isVar(Token);

	bool ExitOperator(Tree*, std::string, size_t);

	void NULLOP(Tree*, std::string, size_t);

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