#include <iostream>
#include "lib/Token.h"
#include "lib/List.h"
#include "lib/Tree.h"
#include "Module/ErrorProcessor.h"
#include "Module/Generator.h"
#include "Module/Lexer.h"
#include "Module/Syntax.h"

int main() {
	ErrorProcessor* ep = new ErrorProcessor();
	Token* t = new Token(3);
	List<int>* l = new List<int>(1, 2, 3);
	for (size_t idx = 0; idx < l->Length(); ++idx)
		std::cout << l->At(idx);
	return 0;
}