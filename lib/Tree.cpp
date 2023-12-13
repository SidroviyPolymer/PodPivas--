#include "Tree.h"

Tree::Tree() {
	data = "";
	left = nullptr;
	right = nullptr;
}

Tree::Tree(std::string& data) {
	this->data = data;
	left = nullptr;
	right = nullptr;
}

Tree::Tree(Tree* tree) : left(nullptr), right(nullptr) {
	data = tree->data;
	if (tree->GetLeft() != nullptr)
		left = new Tree(tree->GetLeft());
	if (tree->GetRight() != nullptr)
		right = new Tree(tree->GetRight());
	//std::cout << data << ":" << std::endl;
	//std::cout << "L" << left << "\t" << tree->GetLeft() << std::endl;
	//std::cout << "R" << right << "\t" << tree->GetRight() << std::endl;
}

Tree::~Tree() {
	if (left != nullptr) {
		delete left;
		left = nullptr;
	}		
	if (right != nullptr) {
		delete right;
		right = nullptr;
	}		
}

void Tree::SetLeft(Tree* tree) {
	if (left != nullptr)
		delete left;

	left = tree;
}

Tree* Tree::GetLeft() {
	//if (left == nullptr)
	//	throw std::exception("Tree: left subtree doesn't exists");
	return left;
}

void Tree::SetRight(Tree* tree) {
	if (right != nullptr)
		delete right;

	right = tree;
}

Tree* Tree::GetRight() {
	//if (right == nullptr)
	//	throw std::exception("Tree: right subtree doesn't exists");
	return right;
}

std::string& Tree::GetData() {
	return data;
}

void Tree::SetData(std::string data) {
	this->data = data;
}

Tree* Tree::CreateLeft() {
	if (left != nullptr)
		throw std::exception("Tree: left subtree isn't empty");
	left = new Tree();
	return left;
}

Tree* Tree::CreateLeft(std::string& data) {
	if (left != nullptr)
		throw std::exception("Tree: left subtree isn't empty");
	left = new Tree(data);
	return left;
}

Tree* Tree::CreateRight() {
	if (right != nullptr)
		throw std::exception("Tree: right subtree isn't empty");
	right = new Tree();
	return right;
}

Tree* Tree::CreateRight(std::string& data) {
	if (right != nullptr)
		throw std::exception("Tree: right subtree isn't empty");
	right = new Tree(data);
	return right;
}

void Tree::DeleteLeft() {
	if (left != nullptr) {
		delete left;
		left = nullptr;
	}		
}

void Tree::DeleteRight() {
	if (right != nullptr) {
		delete right;
		right = nullptr;
	}
}

void Tree::Print() {
	PrintTree(this, 0);
}

void Tree::PrintTree(Tree* tree, size_t level) {
	if (tree == nullptr) {
		for (size_t i = 0; i < level; ++i)
			std::cout << "\t";
		std::cout << "_NOTHING_" << std::endl;
		return;
	}		

	for (size_t i = 0; i < level; ++i)
		std::cout << "\t";
	std::cout << tree->data << std::endl;
	if (tree->left != nullptr || tree->right != nullptr) {
		PrintTree(tree->left, level + 1);
		PrintTree(tree->right, level + 1);
	}
}