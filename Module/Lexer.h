#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "../lib/List.hpp"

class Lexer {
private:
	std::string flow;
public:
	Lexer();

	bool OpenFile(std::string src);
};