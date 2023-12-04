#include "Generator.h"
#include <string>

using namespace std;

Generator::Generator(List<ID>* ids, Tree* syntaxTree)
{
	this->ids = ids;
	this->syntaxTree = syntaxTree;
}

void Generator::Process()
{

	node = GetNextNode();
	string data = node.GetData();		//�������� ����
	if (data.substr(0, 2) == "id")		//���� ������ � ���� ��� �������������, �� �������� �� �������
	{
		data = data.substr(2);
		Generator::ProcessInd(stoi(data));
	}
	else
	{
		if ((data == "+") or (data == "-") or (data == "*") or (data == "/"))
		{
			//����� ������� ��� ��������
		}
		else { if (data == "const") Constant(); if (data == "var") Variable(); }
	}
	///////////////////////////////�� ������ �� �������////////////////////////////////////////////////
	cout << "mov ax,4C00h" << endl;
	cout << "int 21h" << endl;
	if ((ids->At(0).GetType() == 3))
	{
		cout << "end " << ids->At(0).GetContent();
	}
}

Tree Generator::GetNextNode()
{
	Tree node_local;
	node_local = syntaxTree->GetData();
	return node_local;
}

void Generator::ProcessInd(int index)
{
	switch (ids->At(index).GetType())
	{
	case 0:
		//������ � �����������
		break;
	case 1:
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
}

void Generator::Constant()
{
	cout << "constant()";
}

void Generator::Variable()
{
	cout << "variable()";
}
