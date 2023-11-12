#include "Lexer.h"

Lexer::Lexer() {
	flow = new List<std::string>();
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
		
		std::string word = "";

		for (size_t idx = 0; idx < tmp.size(); ++idx) {			
			if (tmp[idx] >= 'A' && tmp[idx] <= 'Z') {
				word += std::tolower(tmp[idx]);
				continue;
			}			

			if (tmp[idx] == ',' || (tmp[idx] == ':' && tmp[idx + 1] != '=') || tmp[idx] == ';') {
				if (word != "")
					flow->Push_back(word);
				word = tmp[idx];
				flow->Push_back(word);
				word = "";
				continue;
			}

			if (tmp[idx] == ' ' || tmp[idx] == '\t') {
				if (word != "")
					flow->Push_back(word);
				word = "";
				continue;
			}			

			word += tmp[idx];
		}	

		if (word != "")
			flow->Push_back(word);
	}

	for (size_t idx = 0; idx < flow->Length(); ++idx)
		std::cout << flow->At(idx) << std::endl;

	return true;
}