#include <iostream>
#include <string>
#include <stack>

using namespace std;

struct TreeNode
{
    string data;
    TreeNode* left;
    TreeNode* right;
    TreeNode(string data):data(data), left(nullptr), right(nullptr){}
};
class Tree
{
private:
    string TreeString(TreeNode* node)
    {
        string leftStr = (node->left == nullptr) ? "{}" : TreeString(node->left);
        string rightStr = (node->right == nullptr) ? "{}" : TreeString(node->right);
        string result = "{" + node->data + leftStr + rightStr + "}";
        return result;
    };

public:
    TreeNode* Root;
    Tree() :Root(nullptr) {}
    Tree(TreeNode* RootNode) :Root(RootNode) {}
    void Print()
    {
        if (this->Root == nullptr)
        {
            cout << "{}" << endl;
        }
        else
        {
            cout << this->TreeString(this->Root) << endl;
        }
    };
};

void Expression(Tree tree)
{
    string op;
    if (tree.Root->data == "+")
    {
        op = int(tree.Root->left->left->data) + int(tree.Root->left->right->data)
    }
}

void Term(string op)
{
    cout << "mov " << "ax, " << op << endl;
}

int main()
{
    TreeNode root("+");
    TreeNode op("op");
    TreeNode op1("5");
    TreeNode op2("6");
    root.left = &op;
    op.left = &op1;
    op.right = &op2;
    Tree myTree{&root};
    myTree.Print();
    Expression(myTree);
    cout << "mov ax,4C00h" << endl;
    cout << "int 21h";
}


