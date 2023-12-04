#include "../lib/List.hpp"
#include "../lib/ID.h"
#include "../lib/Tree.h"

class Generator
{
private:
	List<ID>* ids;
	Tree* syntaxTree;
	Tree node;
	Tree GetNextNode();
	void ProcessInd(int index);
	void Constant();
	void Variable();
public:
	Generator(List<ID>* ids, Tree* syntaxTree);
	void Process();
};

