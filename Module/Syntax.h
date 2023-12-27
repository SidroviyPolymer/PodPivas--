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
	bool DefineProcName(Token&, std::string);
	bool Semicolon(Token);

	bool Block(Tree*, std::string, std::string);

	bool ConstantSection(Tree*, std::string);
	bool DefinitionConstant(Tree*, std::string);
	bool ConstantExpression(Tree*, std::string, int&);
	bool GetConstPostfix(List<Token>*, std::string);
	bool Constant(Token);

	bool VariableSection(Tree*, std::string);
	bool DescriptionSimilarVar(Tree*, std::string, size_t);

	bool ProcedureSection(Tree*, std::string);
	bool ParamSection(Tree*, std::string&, size_t&, std::string);
	bool ParamDescription(Tree*, std::string&, size_t&, std::string);
	bool isProcedure(Token);
	bool CheckProcName(ID&, size_t);

	bool OperatorsSection(Tree*, std::string, std::string, size_t);
	bool CompoundOperator(Tree*, std::string, std::string, size_t);
	bool Operator(Tree*, std::string, std::string, size_t);

	bool SimpleOperator(Tree*, std::string, std::string, size_t);
	bool AssigmentOperator(Tree*, std::string, std::string, size_t);
	bool Expression(Tree*, std::string);
	bool GetPostfix(List<Token>*, std::string);
	bool GetExpressionTree(Tree*, List<Token>*);
	bool isVar(Token);

	bool ProcedureOperator(Tree*, std::string, std::string, size_t);

	bool ComplexOperator(Tree*, std::string, std::string, size_t);

	bool ExitOperator(Tree*, std::string, size_t);

	bool Null(Tree* tree, std::string label, size_t idx);
	void NULLOP(Tree*, std::string, size_t);
	
	bool CheckArea(Token&, std::string);

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