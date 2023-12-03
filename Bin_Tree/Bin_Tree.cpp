#include <iostream>

using namespace std;

//структура узла дерева
typedef struct Node_Tree
{
	string data;
	struct Node_Tree* left;
	struct Node_Tree* right;
} Node_Tree;

Node_Tree* create_tree_element(string i)
{
	Node_Tree* element = new Node_Tree;
	element->data = i;
	element->left = NULL;
	element->right = NULL;
	return element;
}

void print_tree(Node_Tree* cur_tree);

void main()
{

};