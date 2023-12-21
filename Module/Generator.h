#include "../lib/List.hpp"
#include "../lib/ID.h"
#include "../lib/Tree.h"
#include <string>
#include <iostream>

class Generator
{
private:
	List<ID>* ids;
	Tree* syntaxTree;
	Tree node;
	void Constant(Tree* tree);
	void ConstantProcess(Tree* tree);
	void Variable(Tree* tree);
	void VariableProcess(Tree* tree);
	void Operator(Tree* tree);
	void OperatorProcess(Tree* tree);
	void Procedure(Tree* tree);
	void ProcedureProcess(Tree* tree);
	void Process(Tree* tree);
	void Start();
	void TreeTraversal(Tree* tree);
	void Finish();
	Tree* _const;
	Tree* _var;
	Tree* _code;
	Tree* _procedure;
	std::string _name;
	bool exit_flag = 0;
public:
	Generator(List<ID>* ids, Tree* syntaxTree);
	void Working();
};

