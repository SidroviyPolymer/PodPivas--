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

	void SetLeft(Tree*);
	Tree* GetLeft();
	void SetRight(Tree*);
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
