#include "Lexer.h"

Lexer::Lexer() {
	flow = "";
}

bool Lexer::OpenFile(std::string src) {
	std::ifstream is;
	is.open(src);

	bool isOpen = is.is_open();

	if (!isOpen)
		return false;

	while (!is.eof()) {
		std::string tmp;
		is >> tmp;
		flow = flow + " " + tmp;
	}

	std::cout << flow;

	return true;
}
