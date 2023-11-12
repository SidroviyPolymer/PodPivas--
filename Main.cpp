#include <iostream>
#include "lib/Token.h"
#include "lib/List.hpp"
#include "lib/Tree.hpp"
#include "Module/ErrorProcessor.h"
#include "Module/Generator.h"
#include "Module/Lexer.h"
#include "Module/Syntax.h"
#include "lib/ID.hpp"

int main() {
	Lexer* lx = new Lexer();
	std::string fileSrc = "prog.txt";
	lx->OpenFile("../" + fileSrc);
	return 0;
}