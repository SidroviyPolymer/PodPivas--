#include "Lexer.h"

Lexer::Lexer() {
	flow = new List<std::string>();
}

bool Lexer::Process(List<Token>* tlptr, List<std::string>* ilptr, std::string src) {
	tokens = tlptr;
	ids = ilptr;

	if (!OpenFile(src))
		return false;

	Parse();
	TokenList();

	return true;
}

bool Lexer::OpenFile(std::string src) {
	is.open(src);

	return is.good();
}

void Lexer::Parse() {
	std::string word = "";
	bool isCommented = false;
	char prev;
	bool isBegin = true;
	std::string commentBegin;
	while (!is.eof()) {
		char tmp;
		tmp = is.get();

		//if (tmp != '\n')
		//	std::cout << tmp;
		//else
		//	std::cout << "\\n";

		if (isCommented && (tmp == '\n' || !isBegin && tmp == ')' && prev == '*' || tmp == '}')) {
			if (commentBegin == "//") {
				if (tmp == '\n') {
					isCommented = false;
					prev = tmp;
					continue;
				}
			}
			else if (commentBegin == "(*") {
				if (tmp == ')') {
					isCommented = false;
					prev = tmp;
					continue;
				}
			}
			else {
				if (tmp == '}') {
					isCommented = false;
					prev = tmp;
					continue;
				}
			}
		}		

		if (isCommented) {
			prev = tmp;
			continue;
		}

		if (!isBegin && (tmp == '/' && prev == '/' || tmp == '*' && prev == '(') || tmp == '{') {
			isCommented = true;

			if (tmp == '/' || tmp == '*')
				word = word.substr(0, word.length() - 1);

			if (word != "")
				flow->Push_back(word);

			switch (tmp) {
			case '/': commentBegin = "//"; break;
			case '*': commentBegin = "(*"; break;
			case '{': commentBegin = "{"; break;
			}

			prev = tmp;
			continue;
		}

		if (tmp == ':' || tmp == ',' || tmp == ';' || tmp == '(' || tmp == ')') {
			if (word != "")
				flow->Push_back(word);
			word = tmp;
			if (word != ":" && word != "(") {
				flow->Push_back(word);
				word = "";
			}				
			prev = tmp;
			isBegin = false;
			continue;
		}

		if (!isBegin && prev == '(') {
			flow->Push_back(word);
			word = "";
		}

		if (!isBegin && prev == ':') {
			if (tmp == '=') {
				word += '=';
				flow->Push_back(word);
				prev = tmp;
				word = "";
				continue;
			}
			else {
				flow->Push_back(word);
				word = "";
			}
		}

		if (tmp == '\n' || tmp == ' ' || tmp == '\t') {
			if (word != "") {
				flow->Push_back(word);
				word = "";
			}				
			prev = tmp;
			isBegin = false;
			continue;
		}

		if (tmp >= 'A' && tmp <= 'Z')
			tmp = std::tolower(tmp);

		word += tmp;		

		if (word == "end.") {
			flow->Push_back(word);
			break;
		}

		prev = tmp;
		isBegin = false;
	}

	for (size_t idx = 0; idx < flow->Length(); ++idx)
		std::cout << flow->At(idx) << std::endl;
}

void Lexer::TokenList() {
	for (size_t idx = 0; idx < flow->Length(); ++idx) {
		std::string elem = flow->At(idx);
		if (terminals.Contains(elem)) {
			Terminal(elem);
			continue;
		}			
		if (elem[0] >= 'a' && elem[0] <= 'z' || elem[0] == '_') {
			Id(elem);
			continue;
		}
		if (operations.Contains(elem)) {
			Operation(elem);
			continue;
		}
		if (elem[0] >= '0' && elem[0] <= '9' || elem[0] == '-' || elem[0] == '$') {
			Constant(elem);
			continue;
		}

		//Обработка исключения: неверно составленный идентификатор
	}
}

void Lexer::Id(std::string word) {
	size_t idx = 0;
	if (!ids->Contains(word)) {
		ids->Push_back(word);
		idx = ids->Length() - 1;
	}
	else
		idx = ids->Find(word);

	Token* tmp = new Token("id" + std::to_string(idx), Token::Type::Id);
	tokens->Push_back(*tmp);
}

void Lexer::Terminal(std::string word) {
	Token* tmp = new Token(word, Token::Type::Terminal);
	tokens->Push_back(*tmp);
}

void Lexer::Operation(std::string word) {
	Token* tmp = new Token(word, Token::Type::Operation);
	tokens->Push_back(*tmp);
}

void Lexer::Constant(std::string word) {
	int constant = 0;
	if (word[0] != '$') {		
		for (size_t idx = 0; idx < word.length(); ++idx) {
			if (idx == 0 && word[idx] == '-')
				continue;

			if (word[idx] >= '0' && word[idx] <= '9')
				constant = constant * 10 + word[idx] - '0';
			else
				std::cout << "AHTUNG!!!!" << std::endl;
		}			

		if (word[0] == '-')
			constant *= -1;
	}
	else {
		bool isNegative = false;
		for (size_t idx = 1; idx < word.length(); ++idx) {
			if (idx == 1 && (word[idx] == '8' || word[idx] == '9' || word[idx] >= 'a' && word[idx] <= 'f')) {
				isNegative = true;
				constant = constant * 16 + ((word[idx] >= '0' && word[idx] <= '9') ? word[idx] - '0' - 8: word[idx] - 'a' + 2);
				continue;
			}				

			if (word[idx] >= '0' && word[idx] <= '9' || word[idx] >= 'a' && word[idx] <= 'f')
				constant = constant * 16 + ((word[idx] >= '0' && word[idx] <= '9') ? word[idx] - '0' : word[idx] - 'a' + 10);
			else
				std::cout << "AHTUNG!!!!" << std::endl;
		}

		if (isNegative)
			constant -= 32768;
	}

	if (constant < -32768 || constant > 32767)
		std::cout << "AHTUNG!!!! OUT OF BOUNDS" << std::endl;
	else {
		Token* tmp = new Token(std::to_string(constant), Token::Type::Const);
		tokens->Push_back(*tmp);
	}		
}