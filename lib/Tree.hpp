#pragma once
#include <iostream>
#include <exception>

class Tree
{
private:
	std::string data;
	Tree* left;
	Tree* right;

	void PrintTree(Tree*, size_t);

public:
	Tree();
	Tree(std::string& data);
	~Tree();

	Tree* GetLeft();
	Tree* GetRight();

	std::string& GetData();
	void SetData(std::string);

	Tree* CreateLeft();
	Tree* CreateLeft(std::string& data);
	Tree* CreateRight();
	Tree* CreateRight(std::string& data);

	void DeleteLeft();
	void DeleteRight();

	void Print();
};

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

Tree::~Tree() {
	if (left != nullptr)
		delete left;
	if (right != nullptr)
		delete right;
}

Tree* Tree::GetLeft() {
	if (left == nullptr)
		throw std::exception("Tree: left subtree doesn't exists");
	return left;
}

Tree* Tree::GetRight() {
	if (right == nullptr)
		throw std::exception("Tree: right subtree doesn't exists");
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
	if (left != nullptr)
		delete left;
}

void Tree::DeleteRight() {
	if (right != nullptr)
		delete right;
}

void Tree::Print() {
	PrintTree(this, 0);
}

void Tree::PrintTree(Tree* tree, size_t level) {
	if (tree == nullptr)
		return;
	
	for (size_t i = 0; i < level; ++i)
		std::cout << "\t";
	std::cout << data << std::endl;
	PrintTree(left, level + 1);
	PrintTree(right, level + 1);
}
