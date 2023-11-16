#include "Lexer.h"


Lexer::Lexer() {
	flow = new List<std::string>();
	flow2 = new List<Pos>();
}

bool Lexer::Process(List<Token>* tlptr, List<std::string>* ilptr, std::string src) {
	tokens = tlptr;
	ids = ilptr;
	bool result = true;

	if (!OpenFile(src))
		return false;

	Parse();
	TokenList(result);

	std::cout << "result = " << result;
	if (result == true) {
		return true;
	}
	else {
		return false;
	}
}

bool Lexer::OpenFile(std::string src) {
	is.open(src);

	return is.good();
}

void Lexer::Parse() {
	std::string word = "";
	int debugiter = 0;
	int line = 1;
	int column = 0;
	bool isCommented = false;
	char prev;
	bool isBegin = true;
	std::string commentBegin;
	while (!is.eof()) {
		column++;
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
					line++;
					column = 0;
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
				//Pos* buf = new Pos(line, column - word.length());
				//flow2->Push_back(*buf);

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
				Pos* buf = new Pos(line, column - word.length());
				flow2->Push_back(*buf);

			word = tmp;
			if (word != ":" && word != "(") {
				flow->Push_back(word);
				if (prev != ')') {
					Pos* buf2 = new Pos(line, column);
					flow2->Push_back(*buf2);
				}
				word = "";
			}	

			prev = tmp;
			isBegin = false;
			continue;
		}

		if (!isBegin && prev == '(') {
			flow->Push_back(word);
			Pos* buf2 = new Pos(line, column-1);
			flow2->Push_back(*buf2);
			word = "";
		}

		if (!isBegin && prev == ':') {
			if (tmp == '=') {
				word += '=';
				flow->Push_back(word);
				//Pos* buf2 = new Pos(line, column-1);
				//flow2->Push_back(*buf2);
				prev = tmp;
				word = "";
				continue;
			}
			else {
				flow->Push_back(word);
				//Pos* buf2 = new Pos(line, column-1);
				//flow2->Push_back(*buf2);
				word = "";
			}
		}

		if (tmp == '\n' || tmp == ' ' || tmp == '\t') {
			if (word != "") {
				flow->Push_back(word);
				Pos* buf = new Pos(line, column - word.length());
				flow2->Push_back(*buf);
				word = "";
			}	
			if (tmp == '\n') {
				line++;
				column = 0;
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
			Pos* buf = new Pos(line, column + 1 - word.length());
			flow2->Push_back(*buf);
			break;
		}


		prev = tmp;
		isBegin = false;
	}

	for (size_t idx = 0; idx < flow->Length(); ++idx)
		std::cout << flow->At(idx) << std::endl;

	for (size_t idx = 0; idx < flow2->Length(); ++idx)
		std::cout << flow2->At(idx).GetLine() << " " << flow2->At(idx).GetColumn() << std::endl;

}

void Lexer::TokenList(bool& result) {
	for (size_t idx = 0; idx < flow->Length(); ++idx) {
		std::string elem = flow->At(idx);
		Pos elempos = flow2->At(idx);
		if (terminals.Contains(elem)) {
			Terminal(elem,elempos);
			continue;
		}			
		if (elem[0] >= 'a' && elem[0] <= 'z' || elem[0] == '_') {
			Id(elem,elempos);
			continue;
		}
		if (operations.Contains(elem)) {
			Operation(elem, elempos);
			continue;
		}
		if (elem[0] >= '0' && elem[0] <= '9' || elem[0] == '-' || elem[0] == '$') {
			Constant(elem, elempos,result);
			continue;
		}

		else {
			std::cout << "WRONG WORD Line : " << elempos.GetLine() << " Column : " << elempos.GetColumn() << std::endl;
			result = false;
		}
	}
}

void Lexer::Id(std::string word, Pos elempos) {
	size_t idx = 0;
	if (!ids->Contains(word)) {
		ids->Push_back(word);
		idx = ids->Length() - 1;
	}
	else
		idx = ids->Find(word);

	Token* tmp = new Token("id" + std::to_string(idx), Token::Type::Id, elempos.GetLine(), elempos.GetColumn());
	tokens->Push_back(*tmp);
}

void Lexer::Terminal(std::string word, Pos elempos) {
	Token* tmp = new Token(word, Token::Type::Terminal, elempos.GetLine(), elempos.GetColumn());
	tokens->Push_back(*tmp);
}

void Lexer::Operation(std::string word, Pos elempos) {
	Token* tmp = new Token(word, Token::Type::Operation, elempos.GetLine(), elempos.GetColumn());
	tokens->Push_back(*tmp);
}

void Lexer::Constant(std::string word, Pos elempos,bool& result) {
	int constant = 0;
	if (word[0] != '$') {		
		for (size_t idx = 0; idx < word.length(); ++idx) {
			if (idx == 0 && word[idx] == '-')
				continue;

			if (word[idx] >= '0' && word[idx] <= '9')
				constant = constant * 10 + word[idx] - '0';
			else {
				std::cout << "ERROR CONSTANT Line:" << elempos.GetLine() << " Column: " << elempos.GetColumn() << std::endl;
				result = false;
			}
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
			else {
				std::cout << "ERROR CONSTANT Line:" << elempos.GetLine() << " Column: " << elempos.GetColumn() << std::endl;
				result = false;
			}
		}

		if (isNegative)
			constant -= 32768;
	}

	if (constant < -32768 || constant > 32767) {
		std::cout << "OUT OF BOUNDS Line:" << elempos.GetLine() << " Column: " << elempos.GetColumn() << std::endl;
		result = false;
	}
	else {
		Token* tmp = new Token(std::to_string(constant), Token::Type::Const, elempos.GetLine(), elempos.GetColumn());
		tokens->Push_back(*tmp);
	}		
}