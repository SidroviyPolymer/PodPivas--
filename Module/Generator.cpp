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
	if (data == "var") _var = tree->GetLeft(); 
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
			if (tree->GetData() == ":=")		//pop id
			{
				int index = stoi(tree->GetLeft()->GetData().substr(2));
				cout << "pop AX" << endl;
				cout << "pop BX" << endl;
				cout << "mov " << ids->At(index).GetContent() << ", AX" << endl;
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
			if ((tree->GetData() != "NULL") and (tree->GetData().substr(0, 2) != "OP") and (tree->GetData() != "exit"))
			{
				cout << "push " << tree->GetData() << endl;
			}
		}
	}	
	if (tree->GetData() == "exit")
	{
		cout << "jmp exit" << endl;
		exit_flag = 1;
	}
}

void Generator::ProcedureTreeTraversal(Tree* tree)		
{
	
}

void Generator::Procedure(Tree* tree)	//нужно подумать о порядке обхода
{
	if (tree->GetLeft() != nullptr) Procedure(tree->GetLeft());
	if (tree->GetRight() != nullptr) Procedure(tree->GetRight());
	ProcedureProcess(tree);
}

void Generator::ProcedureProcess(Tree* tree)	//наверное нужно работать как с основной прогой
{

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
	cout << ".code" << endl;		
	cout << _name << ":" << endl;
	cout << "mov ax, @Data" << endl;
	cout << "mov ds, ax" << endl;
	if (_code != nullptr) Operator(_code);
	if (exit_flag == 1) cout << "exit:" << endl;
	cout << "mov ax,4C00h" << endl;
	cout << "int 21h" << endl;
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