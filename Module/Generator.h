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
	void ProcedureID(Tree* tree);
	void ProcedureProcess(Tree* tree, int i);
	void ProcedureBlock(Tree* tree, int i);
	void Process(Tree* tree);
	void ProcedureParamTrav(Tree* tree);
	void ProcedureParamTravMain(Tree* tree);
	void Start();
	void TreeTraversal(Tree* tree);
	void Finish();
	bool found(std::string a);
	Tree* _const;
	Tree* _var;
	Tree* _code;
	Tree* _procedure;
	//штуки для процедур
	Tree* _param;
	Tree* procedure_address[10];	//адреса процедур
	Tree* var_proc[10];		
	Tree* block_proc[10];
	Tree* var_param_proc[10];
	std::string var_proc_id[10];	//локальные id процедур для резервирования памяти
	std::string var_param_proc_id[10];
	std::string var_param_main[10];	//передаваемые параметры
	std::string var_param_address[10]; //список ссылок
	int var_param_address_count = 0;
	int pr_count_proc = 0;
	int pr_count_var = 0;
	int pr_count_block = 0;
	int pr_count_var_id = 0;
	int pr_count_var_param = 0;
	int pr_count_var_param_id = 0;
	int pr_count_var_param_main = 0;
	////////////////
	std::string stack_out = "";
	std::string _name;
	bool exit_flag = 0;
	bool exit_flag_proc = 0;
	int i_oper = 0;
	bool proc = 0;
public:
	Generator(List<ID>* ids, Tree* syntaxTree);
	void Working();
};

