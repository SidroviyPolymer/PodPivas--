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
	List<Token>* tokens = new List<Token>();
	List<std::string>* ids = new List<std::string>();
	lx->Process(tokens, ids, "../" + fileSrc);

	std::cout << std::endl << "<<------------------------->>" << std::endl << std::endl;

	for (size_t idx = 0; idx < tokens->Length(); ++idx) {
		std::cout << tokens->At(idx) << std::endl;
	}
	for (size_t idx = 0; idx < ids->Length(); ++idx) {
		std::cout << ids->At(idx) << std::endl;
	}
	return 0;
}