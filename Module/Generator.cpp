#include "Generator.h"
#include <string>

using namespace std;

Generator::Generator(List<ID>* ids, Tree* syntaxTree)
{
	this->ids = ids;
	this->syntaxTree = syntaxTree;
}

void Generator::TreeTraversal(Tree* tree)		//проходим все синтаксическое дерево сохран€€ адреса поддеревьев
{
	Process(tree);
	if (tree->GetLeft() != nullptr) TreeTraversal(tree->GetLeft());
	if (tree->GetRight() != nullptr) TreeTraversal(tree->GetRight());
	//cout << tree->GetData() << endl;
}

void Generator::Process(Tree* tree)		
{
	string data = tree->GetData();		
	if (tree->GetData().substr(0, 2) == "id")
	{
		int index = stoi(tree->GetData().substr(2));
		if (ids->At(index).GetType() == 3)
		{
			_name = ids->At(index).GetContent();
			_code = tree->GetRight();
		}
	}
	if (data == "const") _const = tree->GetLeft();
	if (_var == nullptr)
	{
		if (data == "var") _var = tree->GetLeft();
	}
	else
	{
		if (tree->GetData() == "var")
		{
			var_proc[pr_count_var] = tree->GetLeft();
			pr_count_var += 1;
		}
	}
	if (data == "procedure") _procedure = tree->GetLeft();
	if (tree->GetData() == "block")
	{
		block_proc[pr_count_block] = tree->GetRight();
		pr_count_block += 1;
	}
	if (tree->GetData() == "var-param")
	{
		var_param_proc[pr_count_var_param] = tree->GetLeft();
		pr_count_var_param += 1;
	}
}

void Generator::Constant(Tree* tree)
{
	ConstantProcess(tree);
	if (tree->GetLeft() != nullptr) Constant(tree->GetLeft());
	if (tree->GetRight() != nullptr) Constant(tree->GetRight());
	//cout << tree->GetData() << endl;
}

void Generator::ConstantProcess(Tree* tree)		
{
	if (tree->GetLeft() != nullptr)
	{
		int index = stoi(tree->GetData().substr(2));
		cout << ids->At(index).GetContent() << " EQU " << tree->GetLeft()->GetData() << endl;		
	}
}

void Generator::Variable(Tree* tree)
{
	VariableProcess(tree);
	if (tree->GetLeft() != nullptr) Variable(tree->GetLeft());
	if (tree->GetRight() != nullptr) Variable(tree->GetRight());
}

void Generator::VariableProcess(Tree* tree)
{
	if (tree->GetData().substr(0, 2) == "id")
	{
		int index = stoi(tree->GetData().substr(2));
		cout << ids->At(index).GetContent() << " dw " << "?" << endl;
	}
}

void Generator::Operator(Tree* tree)
{
	if (tree->GetLeft() != nullptr) Operator(tree->GetLeft());
	if (tree->GetRight() != nullptr) Operator(tree->GetRight());
	OperatorProcess(tree);
	if (tree->GetData() == "exit")	
	{
		cout << "jmp exit" << endl;
		exit_flag = 1;
	}
	if (tree->GetData() == "CALL")
	{
		_param = tree->GetRight();
		ProcedureParamTravMain(_param);
		ProcedureParamTrav(var_param_proc[i_oper]);
		i_oper += 1;
		for (int i = 0; i < pr_count_var_param_main; i++)
		{
			cout << "pop CX" << endl;
			cout << "lea BX, " << var_param_main[i] << endl;
			cout << "mov " << var_param_proc_id[i] << ", BX" << endl;
			var_param_address[var_param_address_count] = var_param_proc_id[i];
			var_param_address_count += 1;
		}
		int index = stoi(tree->GetLeft()->GetData().substr(2));
		cout << "call " << ids->At(index).GetContent() << endl;
		pr_count_var_param_main = 0;
		pr_count_var_param_id = 0;
	}
}

void Generator::ProcedureParamTrav(Tree* tree)
{
	if (tree->GetLeft() != nullptr) ProcedureParamTrav(tree->GetLeft());
	if (tree->GetRight() != nullptr) ProcedureParamTrav(tree->GetRight());
	if (tree->GetData().substr(0, 2) == "id")
	{
		int index = stoi(tree->GetData().substr(2));
		var_param_proc_id[pr_count_var_param_id] = ids->At(index).GetContent();
		pr_count_var_param_id += 1;
	}
}

void Generator::ProcedureParamTravMain(Tree* tree)
{
	if (tree->GetLeft() != nullptr) ProcedureParamTravMain(tree->GetLeft());
	if (tree->GetRight() != nullptr) ProcedureParamTravMain(tree->GetRight());
	if (tree->GetData().substr(0, 2) == "id")
	{
		int index = stoi(tree->GetData().substr(2));
		var_param_main[pr_count_var_param_main] = ids->At(index).GetContent();
		pr_count_var_param_main += 1;
	}
}

void Generator::OperatorProcess(Tree* tree)
{
	if (tree->GetData().substr(0, 2) == "id")
	{
		int index = stoi(tree->GetData().substr(2));
		if (ids->At(index).GetContent().substr(0, 1) != "@")
		{
			if (proc == 1)
			{
				if (found(ids->At(index).GetContent()))
				{
					cout << "push [" << ids->At(index).GetContent() << "]" << endl;
				}
				else cout << "push " << ids->At(index).GetContent() << endl;
			}
			else cout << "push " << ids->At(index).GetContent() << endl;
		}
	}
	else
	{
		if ((tree->GetData() == "+") or (tree->GetData() == "*") or (tree->GetData() == ":=") or (tree->GetData() == "-") or (tree->GetData() == "div"))
		{
			if (tree->GetData() == "+")
			{
				cout << "pop BX" << endl;
				cout << "pop AX" << endl;
				cout << "add AX, BX" << endl;
				cout << "push AX" << endl;
			}
			if (tree->GetData() == "*")
			{
				cout << "pop BX" << endl;
				cout << "pop AX" << endl;
				cout << "mul BX" << endl;
				cout << "push AX" << endl;

			}
			if (tree->GetData() == ":=")
			{
				int index = stoi(tree->GetLeft()->GetData().substr(2));
				if (proc == 1)
				{
					if (found(ids->At(index).GetContent()))
					{
						cout << "mov BX, " << ids->At(index).GetContent() << endl;
						cout << "pop [BX]" << endl;
					}
					else cout << "pop " << ids->At(index).GetContent() << endl;
				}
				else
				{
					cout << "pop " << ids->At(index).GetContent() << endl;
				}
				cout << "pop BX" << endl;
			}
			if (tree->GetData() == "-")
			{
				cout << "pop BX" << endl;
				cout << "pop AX" << endl;
				cout << "sub AX, BX" << endl;
				cout << "push AX" << endl;

			}
			if (tree->GetData() == "div")
			{
				cout << "pop BX" << endl;
				cout << "pop AX" << endl;
				cout << "div BX" << endl;
				cout << "push AX" << endl;
			}
		}
		else
		{
			if ((tree->GetData() != "NULL") and (tree->GetData().substr(0, 2) != "OP") and (tree->GetData() != "exit") and (tree->GetData().substr(0, 4) != "PROC") and (tree->GetData() != "CALL") and (tree->GetData() != ""))
			{
				cout << "push " << tree->GetData() << endl;
			}
		}
	}	
	
}

bool Generator::found(string a)
{
	for (int i = 0; i < var_param_address_count; i++)
	{
		if (a == var_param_address[i]) return 1;
	}
	return 0;
}

void Generator::ProcedureTreeTraversal(Tree* tree)
{
	ProcedureAddress(tree);
	if (tree->GetLeft() != nullptr) ProcedureTreeTraversal(tree->GetLeft());
	if (tree->GetRight() != nullptr) ProcedureTreeTraversal(tree->GetRight());
}

void Generator::ProcedureAddress(Tree* tree)
{
	if ((tree->GetData().substr(0, 2) == "PR") and (tree->GetData().substr(0, 4) != "PROC"))
	{
		procedure_address[pr_count_proc] = tree->GetLeft();
		pr_count_proc += 1;
	}
}

void Generator::ProcedureProcess(Tree* tree, int i)	
{
	int index = stoi(tree->GetData().substr(2));
	cout << ids->At(index).GetContent() << ":" << endl;	
	Tree* block_pr = block_proc[i];
	ProcedureBlock(block_pr, i);
	if (exit_flag_proc == 1)
	{
		cout << "exit_proc_" << i << ":" << endl;
		exit_flag_proc = 0;
	}
	cout << stack_out;
	cout << "ret" << endl;
}

void Generator::ProcedureBlock(Tree* tree, int i)
{
	if (tree->GetLeft() != nullptr) ProcedureBlock(tree->GetLeft(), i);
	if (tree->GetRight() != nullptr) ProcedureBlock (tree->GetRight(), i);
	OperatorProcess(tree);
	if (tree->GetData() == "exit")
	{
		cout << "jmp exit_proc_" << i << endl;
		exit_flag_proc = 1;
	}
	if (tree->GetData() == "CALL")
	{
		int index = stoi(tree->GetLeft()->GetData().substr(2));
		cout << "call " << ids->At(index).GetContent() << endl;
	}
}

void Generator::ProcedureID(Tree* tree)
{
	if (tree->GetLeft() != nullptr) ProcedureID(tree->GetLeft());
	if (tree->GetRight() != nullptr) ProcedureID(tree->GetRight());
	if (tree->GetData().substr(0, 2) == "id")
	{
		int index = stoi(tree->GetData().substr(2));
		string s = ids->At(index).GetContent();
		ids->At(index).GetContent() = s + "_local";
		var_proc_id[pr_count_var_id] = ids->At(index).GetContent();
		pr_count_var_id += 1;
	}
}

void Generator::Start()
{
	var_proc_id[0] = "null";
	cout << ".model tiny" << endl;
	cout << ".stack 200h" << endl;
	cout << ".data" << endl;
	TreeTraversal(syntaxTree);
}

void Generator::Finish()
{
	if (_const != nullptr) Constant(_const);
	if (_var != nullptr) Variable(_var);
	if (var_proc[0] != nullptr)	//сохран€ем id процедур
	{
		for (int i = 0; i < pr_count_var; i++)
		{
			ProcedureID(var_proc[i]);
		}
	}
	if (var_param_proc[0] != nullptr)
	{
		for (int i = 0; i < pr_count_var_param; i++)
		{
			ProcedureID(var_param_proc[i]);
		}
	}
	if (var_proc_id[0] != "null")
	{
		for (int i = 0; i < pr_count_var_id; i++)
		{
			cout << var_proc_id[i] << " dw ?" << endl;
		}
	}
	if (_procedure != nullptr) ProcedureTreeTraversal(_procedure);	//сохран€ем адреса процедур
	cout << ".code" << endl;		
	cout << _name << ":" << endl;
	cout << "mov ax, @Data" << endl;
	cout << "mov ds, ax" << endl;
	if (_code != nullptr) Operator(_code);
	if (exit_flag == 1) cout << "exit:" << endl;
	cout << "mov ax,4C00h" << endl;
	cout << "int 21h" << endl;
	proc = 1;
	if (_procedure != nullptr)	//начинаем генерить процедуры
	{
		for (int i = 0; i < pr_count_proc; i++)
		{
			ProcedureProcess(procedure_address[i], i);
			stack_out = "";
		}
	}
	if ((ids->At(0).GetType() == 3))
	{
		cout << "end " << ids->At(0).GetContent() << endl;
	}
}

void Generator::Working()
{
	Start();
	Finish();
}