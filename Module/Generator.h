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
	//Tree GetNextNode();
	//void ProcessInd(int index);
	void Constant(Tree* tree);
	void ConstantProcess(Tree* tree);
	void Variable(Tree* tree);
	void VariableProcess(Tree* tree);
	void Process(Tree* tree);
	void Start();
	void TreeTraversal(Tree* tree);
	void Finish();
public:
	Tree* _const;
	Tree* _var;
	std::string _name;
	Generator(List<ID>* ids, Tree* syntaxTree);
	void Working();
};

