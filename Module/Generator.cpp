#include "Generator.h"
#include <string>

using namespace std;

Generator::Generator(List<ID>* ids, Tree* syntaxTree)
{
	this->ids = ids;
	this->syntaxTree = syntaxTree;
}

void Generator::TreeTraversal(Tree* tree)		//проходим все синтаксическое дерево сохраняя адреса поддеревьев
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
	if (data == "procedure") _procedure = tree->GetLeft();
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
}

void Generator::OperatorProcess(Tree* tree)
{
	if (tree->GetData().substr(0, 2) == "id")
	{
		int index = stoi(tree->GetData().substr(2));
		cout << "push " << ids->At(index).GetContent() << endl;
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
				cout << "pop " << ids->At(index).GetContent() << endl;
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
			if ((tree->GetData() != "NULL") and (tree->GetData().substr(0, 2) != "OP") and (tree->GetData() != "exit") and (tree->GetData().substr(0, 4) != "PROC"))
			{
				cout << "push " << tree->GetData() << endl;
			}
		}
	}	
	
}

void Generator::ProcedureTreeTraversal(Tree* tree)	//обход возможно неправильный		
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

void Generator::Procedure(Tree* tree)
{
	ProcedureAddressInside(tree);
	if (tree->GetLeft() != nullptr) Procedure(tree->GetLeft());
	if (tree->GetRight() != nullptr) Procedure(tree->GetRight());
}

void Generator::ProcedureAddressInside(Tree* tree)	
{
	if (tree->GetData() == "var")
	{
		var_proc[pr_count_var] = tree->GetLeft();
		pr_count_var += 1;
	}
	if (tree->GetData() == "block")
	{
		block_proc[pr_count_block] = tree->GetRight();
		pr_count_block += 1;
	}
}

void Generator::ProcedureProcess(Tree* tree, int i)	
{
	int index = stoi(tree->GetData().substr(2));
	cout << ids->At(index).GetContent() << ":" << endl;	//нужно засунуть все используемые переменные в стек, а потом вытащить в обратном порядке
	Tree* var_pr = var_proc[i];
	ProcedureVar(var_pr);
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

void Generator::ProcedureVar(Tree* tree)
{
	ProcedureVarProcess(tree);
	if (tree->GetLeft() != nullptr) ProcedureVar(tree->GetLeft());
	if (tree->GetRight() != nullptr) ProcedureVar(tree->GetRight());
}

void Generator::ProcedureVarProcess(Tree* tree)
{
	if (tree->GetData().substr(0, 2) == "id")
	{
		int index = stoi(tree->GetData().substr(2));
		cout << "push " << ids->At(index).GetContent() << endl;
		stack_out ="pop " + ids->At(index).GetContent() + "\n" + stack_out;
 	}
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
}

void Generator::Start()
{
	cout << ".model tiny" << endl;
	cout << ".stack 200h" << endl;
	cout << ".data" << endl;
	TreeTraversal(syntaxTree);
}

void Generator::Finish()
{
	if (_const != nullptr) Constant(_const);
	if (_var != nullptr) Variable(_var);
	if (_procedure != nullptr) ProcedureTreeTraversal(_procedure);
	for (int i = 0; i < pr_count_proc; i++)
	{
		if (procedure_address[i] != nullptr) Procedure(procedure_address[i]);
	}
	cout << ".code" << endl;		
	cout << _name << ":" << endl;
	cout << "mov ax, @Data" << endl;
	cout << "mov ds, ax" << endl;
	if (_code != nullptr) Operator(_code);
	if (exit_flag == 1) cout << "exit:" << endl;
	cout << "mov ax,4C00h" << endl;
	cout << "int 21h" << endl;
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