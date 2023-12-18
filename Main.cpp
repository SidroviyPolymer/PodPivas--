#include <iostream>
#include <fstream>
#include <locale>
#include <exception>
#include "lib/Token.h"
#include "lib/List.hpp"
#include "lib/Tree.h"
#include "lib/Error.h"
#include "Module/ErrorProcessor.h"
#include "Module/Generator.h"
#include "Module/Lexer.h"
#include "Module/Syntax.h"
#include "lib/ID.h"

int main() {	
	try {
		List<Error>* errlist = new List<Error>();

		Lexer* lx = new Lexer();
		std::string fileSrc = "prog.txt";
		List<Token>* tokens = new List<Token>();
		List<ID>* ids = new List<ID>();
		bool lxGood = lx->Process(tokens, ids, fileSrc, errlist);
		if (!lxGood) {
			std::ofstream erros, tokos, idos;
			erros.open("errors.log");
			tokos.open("tokens.log");
			idos.open("idos.log");

			errlist->PrintAllLn(erros);
			tokens->PrintAllLn(tokos);
			ids->PrintAllLn(idos);

			erros.close();
			tokos.close();
			idos.close();

			delete errlist;
			delete lx;
			delete tokens;
			delete ids;
			return 0;
		}

		std::ofstream tokos, idos;
		tokos.open("tokens.log");
		idos.open("idos.log");

		tokens->PrintAllLn(tokos);
		ids->PrintAllLn(idos);

		tokos.close();
		idos.close();


		
		Syntax* sx = new Syntax(tokens, ids, errlist);
		bool sxGood = sx->Process();
		if (!sxGood) {
			return 0;
		}

		Tree* syntaxTree = sx->GetTree();
		syntaxTree->Print();

		idos.open("s_idos.log");
		ids->PrintAllLn(idos);
		idos.close();		

		Generator* gen = new Generator(ids, syntaxTree);
		gen->Working();
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	
	return 0;
}