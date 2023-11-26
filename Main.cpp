#include <iostream>
#include <fstream>
#include <locale>
#include <exception>
#include "lib/Token.h"
#include "lib/List.hpp"
#include "lib/Tree.hpp"
#include "lib/Error.h"
#include "Module/ErrorProcessor.h"
#include "Module/Generator.h"
#include "Module/Lexer.h"
#include "Module/Syntax.h"
#include "lib/ID.hpp"

int main() {	
	try {
		List<Error>* errlist = new List<Error>();

		Lexer* lx = new Lexer();
		std::string fileSrc = "prog.txt";
		List<Token>* tokens = new List<Token>();
		List<std::string>* ids = new List<std::string>();
		bool lxGood = lx->Process(tokens, ids, "../" + fileSrc, errlist);
		if (!lxGood) {
			std::ofstream erros, tokos;
			erros.open("errors.log");
			tokos.open("tokens.log");

			errlist->PrintAllLn(erros);
			tokens->PrintAllLn(tokos);
			delete errlist;
			delete lx;
			delete tokens;
			delete ids;
			return 0;
		}

		Syntax* sx = new Syntax();
		bool sxGood = sx->Process(tokens, ids, errlist);
		if (!sxGood) {

		}
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	
	return 0;
}