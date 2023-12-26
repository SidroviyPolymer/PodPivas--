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
	void ProcedureTreeTraversal(Tree* tree);
	void ProcedureAddress(Tree* tree);
	void ProcedureAddressInside(Tree* tree);
	void Procedure(Tree* tree);
	void ProcedureProcess(Tree* tree, int i);
	void ProcedureVar(Tree* tree);
	void ProcedureVarProcess(Tree* tree);
	void ProcedureBlock(Tree* tree, int i);
	void Process(Tree* tree);
	void Start();
	void TreeTraversal(Tree* tree);
	void Finish();
	Tree* _const;
	Tree* _var;
	Tree* _code;
	Tree* _procedure;
	//штуки для процедур
	Tree* procedure_address[10];
	Tree* var_proc[10];
	Tree* block_proc[10];
	int pr_count_proc = 0;
	int pr_count_var = 0;
	int pr_count_block = 0;
	std::string stack_out = "";
	std::string _name;
	bool exit_flag = 0;
	bool exit_flag_proc = 0;
public:
	Generator(List<ID>* ids, Tree* syntaxTree);
	void Working();
};

