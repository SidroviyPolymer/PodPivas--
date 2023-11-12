#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "../lib/List.hpp"

class Lexer {
private:
	List<std::string>* flow;
public:
	Lexer();

	bool OpenFile(std::string src);
};