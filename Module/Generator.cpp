#include "Generator.h"
#include <string>

using namespace std;

Generator::Generator(List<ID>* ids, Tree* syntaxTree)
{
	this->ids = ids;
	this->syntaxTree = syntaxTree;
}

void Generator::TreeTraversal(Tree* tree)		//�������� ��� �������������� ������ �������� ������ �����������
{
	Process(tree);
	if (tree->GetLeft() != nullptr) TreeTraversal(tree->GetLeft());
	if (tree->GetRight() != nullptr) TreeTraversal(tree->GetRight());
	//cout << tree->GetData() << endl;
}

void Generator::Process(Tree* tree)		 //����� ����������
{
	string data = tree->GetData();		//�������� ����
	if (tree->GetData().substr(0, 2) == "id")
	{
		int index = stoi(tree->GetData().substr(2));
		if (ids->At(index).GetType() == 3) _name = ids->At(index).GetContent();
	}
	if (data == "const") _const = tree->GetLeft();
	if (data == "var") _var = tree->GetLeft(); 


}

/*void Generator::ProcessInd(int index)		//����� ����������
{
	switch (ids->At(index).GetType())
	{
	case 0:
		//������ � �����������
		break;
	case 2:
		//������ � �����������
		break;
	case 3:
		cout << ids->At(index).GetContent() << ":" << endl;
		break;
	default:
		break;
	}
}*/

void Generator::Constant(Tree* tree)
{
	ConstantProcess(tree);
	if (tree->GetLeft() != nullptr) Constant(tree->GetLeft());
	if (tree->GetRight() != nullptr) Constant(tree->GetRight());
	//cout << tree->GetData() << endl;
}

void Generator::ConstantProcess(Tree* tree)		//��������, �� �������� �����
{
	if (tree->GetLeft() != nullptr)
	{
		int index = stoi(tree->GetData().substr(2));
		cout << "constant " << ids->At(index).GetContent() << " dd " << tree->GetLeft()->GetData() << endl;		//��� ����� �������� K EQU 5, ��� � ����� ���������
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
		cout << ids->At(index).GetContent() << " dd " << "?" << endl;
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
	Constant(_const);
	Variable(_var);
	cout << ".code" << endl;		
	cout << _name << ":" << endl;
	cout << "mov ax, @Data" << endl;
	cout << "mov ds, ax" << endl;
	cout << "!!!CODE!!!" << endl;
	cout << "mov ax,4C00h" << endl;
	cout << "int 21h" << endl;
	if ((ids->At(0).GetType() == 3))
	{
		cout << "end " << ids->At(0).GetContent();
	}
}

void Generator::Working()
{
	Start();
	Finish();
}