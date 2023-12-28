#include "Lexer.h"

#include <sstream>

Pos::Pos() {}

Pos::Pos(int Line, int Column) {
	this->Line = Line;
	this->Column = Column;
}

int& Pos::GetLine() {
	return Line;
}

int& Pos::GetColumn() {
	return Column;
}

Pos& Pos::operator=(const Pos& pos) {

	if (this == &pos) {
		return *this;
	}

	Line = pos.Line;
	Column = pos.Column;
	return *this;
}

Pos::Pos(const Pos& p) {
	Line = p.Line;
	Column = p.Column;
}

std::ostream& operator<<(std::ostream& os, Pos const& pos) {
	os << "(" << std::to_string(pos.Line) << ", " << std::to_string(pos.Column) << ")";
	return os;
}


Lexer::Lexer() { // конструктор класса ЛА, создаёт два списка
	flow = new List<std::string>();
	flow2 = new List<Pos>();
}

bool Lexer::Process(List<Token>* tlptr, List<ID>* ilptr, std::string src, List<Error>* errlist) { // Запуск ЛА, на входе два списка и файл с кодом
	tokens = tlptr;
	ids = ilptr;
	this->errlist = errlist;
	bool result = true; // Результат работы ЛА

	if (!OpenFile(src)) // Не получилось открыть
		return false;

	Parse(); // Вызов парсера
	
	std::ofstream f1os, f2os;
	f1os.open("parser.log");
	f2os.open("pos.log");

	flow->PrintAllLn(f1os);
	flow2->PrintAllLn(f2os);

	TokenList(result);

	return result;
}

bool Lexer::OpenFile(std::string src) {
	is.open(src);

	return is.good();
}

void Lexer::Parse() {
	std::string word = "";
	int line = 1; // счётчик строк
	int column = 0; // счётчик стобцов
	bool isCommented = false; // Если идёт обработка комментария
	char prev;  // хранилище предыдущего символа
	bool isBegin = true;  // Указатель начала файла
	std::string commentBegin; // хранилище конкретного типа комментария
	while (!is.eof()) {
		column++;
		char tmp; // текущий символ
		tmp = is.get();

		if (isCommented && (tmp == '\n' || !isBegin && tmp == ')' && prev == '*' || tmp == '}')) { // Если сейчас идёт комментарий
			if (commentBegin == "//") { //Если коммент "//" то конец будет только на следующей строке
				if (tmp == '\n') {
					isCommented = false;
					line++;
					column = 0;
					prev = tmp;
					continue;
				}
			}
			else if (commentBegin == "(*") { //Если коммент "(*" то конец будет только после закрытия скобок "*)"
				if (tmp == ')') {
					isCommented = false;
					prev = tmp;
					continue;
				}
			}
			else {
				if (tmp == '}') {            //Если коммент "{" то конец будет только после закрытия скобок "}"
					isCommented = false;
					prev = tmp;
					continue;
				}
			}
		}		

		if (isCommented) {
			if (tmp == '\n') {
				++line;
				column = 0;
			}				
			prev = tmp;
			continue;
		}

		if (!isBegin && (tmp == '/' && prev == '/' || tmp == '*' && prev == '(') || tmp == '{') { // Если у нас начало коммента, то isCommented = true;
			isCommented = true;

			if (tmp == '/' || tmp == '*')
				word = word.substr(0, word.length() - 1);

			if (word != "")
				flow->Push_back(word);;

			switch (tmp) { // выбор варианта коммента на языке Pascal
			case '/': commentBegin = "//"; break;
			case '*': commentBegin = "(*"; break;
			case '{': commentBegin = "{"; break;
			}

			prev = tmp;
			continue;
		}

		if (tmp == ':' || tmp == ',' || tmp == ';' || tmp == '(' || tmp == ')' || tmp == '+' || tmp == '-' || tmp == '*') { // Если встретили терминальный символ,а не коммент
			if (word != "") {
				flow->Push_back(word);
				Pos buf = Pos(line, column - word.length());
				flow2->Push_back(buf);
			}

			word = tmp;
			if (word != ":" && word != "(") {
				flow->Push_back(word);
				Pos buf2 = Pos(line, column);
				flow2->Push_back(buf2);
				word = "";
			}	

			prev = tmp;
			isBegin = false;
			continue;
		}

		if (!isBegin && prev == '(') { // Если прошлый символ  '(' не часть коммента, то считаем его отдельным токеном
			flow->Push_back(word);
			Pos buf2 = Pos(line, column-1);
			flow2->Push_back(buf2);
			word = "";
		}

		if (!isBegin && prev == ':') {
			if (tmp == '=') { // Если нынешний символ  '=' и образует ":=" то это целый токен, помещаем его
				word += '=';
				flow->Push_back(word);
				Pos buf2 = Pos(line, column - 2);
				flow2->Push_back(buf2);
				prev = tmp;
				word = "";
				continue;
			}
			else { // Если нынешний символ не '=', то считаем ':' отдельным токеном
				flow->Push_back(word);
				Pos buf2 = Pos(line, column - 1);
				flow2->Push_back(buf2);
				word = ""; 
			}
		}

		if (tmp == '=') {
			if (word != "") {
				flow->Push_back(word);
				Pos buf = Pos(line, column - word.length());
				flow2->Push_back(buf);
			}

			word = tmp;
			flow->Push_back(word);
			Pos buf2 = Pos(line, column);
			flow2->Push_back(buf2);
			word = "";

			prev = tmp;
			isBegin = false;
			continue;
		}

		if (tmp == '\n' || tmp == ' ' || tmp == '\t') { //Если слово закончилось, то помещаем в предварительный список токенов и позиций
			if (word != "") { // Если не пустое
				flow->Push_back(word);
				Pos buf = Pos(line, column - word.length());
				flow2->Push_back(buf);
				word = "";
			}	
			if (tmp == '\n') { // Если конец строки, то счётчик строк++, столбцов обнуляется 
				line++;
				column = 0;
			}
			prev = tmp;
			isBegin = false;
			continue;
		}

		if (tmp >= 'A' && tmp <= 'Z') // Приведение к нижнему регистру
			tmp = std::tolower(tmp);

		word += tmp;		

		if (word == "end.") { // Если end. , то прекраща
			std::string end = word.substr(0, word.length() - 1);
			flow->Push_back(end);
			std::string point = ".";
			flow->Push_back(point);;
			Pos buf = Pos(line, column + 1 - word.length());
			buf = Pos(line, column + 1 - word.length());
			flow2->Push_back(buf);
			buf = Pos(line, column);
			flow2->Push_back(buf);
			word = "";
			break;
		}


		prev = tmp;
		isBegin = false;
	}

	if (word != "") {
		word = word.substr(0, word.length() - 1);
		flow->Push_back(word);
		Pos buf = Pos(line, column + 1 - word.length());
		flow2->Push_back(buf);
	}

	//for (size_t idx = 0; idx < flow->Length(); ++idx)
	//	std::cout << idx << ":\t" << flow->At(idx) << std::endl;

	//for (size_t idx = 0; idx < flow2->Length(); ++idx)
	//	std::cout << idx << ":\t" << flow2->At(idx).GetLine() << " " << flow2->At(idx).GetColumn() << std::endl;

}

void Lexer::TokenList(bool& result) { // Функция заполнение листов
	//std::cout << flow->Length() << " " << flow2->Length() << std::endl;
	std::string prev = "";
	Pos prevpos;
	Pos elempos;
	bool isNegative = false;
	for (; flow->Length() > 0;) {
		std::string elem = flow->Pop_front();
		elempos = flow2->Pop_front();
		if (terminals.Contains(elem)) { // Если есть совпадения с терминальными символами
			Terminal(elem,elempos);
			prev = elem;
			continue;
		}

		if (elem[0] == '-') {
			if (prev != "" && ((prev[0] >= 'a' && prev[0] <= 'z' || prev[0] == '_') || (prev[0] >= '0' && prev[0] <= '9' || prev[0] == '$'))) {
				Operation(elem, elempos);
				prev = elem;
				prevpos = elempos;
				continue;
			}
			else {
				isNegative = true;
				prev = elem;
				prevpos = elempos;
				continue;
			}
		}

		if (operations.Contains(elem)) { // Если есть совпадения с оператарами
			Operation(elem, elempos);
			prev = elem;
			prevpos = elempos;
			continue;
		}
		if (elem[0] >= 'a' && elem[0] <= 'z' || elem[0] == '_') { // Если начинается с символа от a до z или _
			if (isNegative) {
				Operation(prev, prevpos);
				isNegative = false;
			}				
			Id(elem,elempos);
			prev = elem;
			prevpos = elempos;
			continue;
		}
		if (elem[0] >= '0' && elem[0] <= '9' || elem[0] == '$') { // Если начинается на цифру, знак
			if (isNegative) {
				elem = elem.insert(0, "-");
				isNegative = false;
			}				
			Constant(elem, elempos, result);
			prev = elem;
			prevpos = elempos;
			continue;
		}

		// Если нет совпадений, то ошибка
		Error err("L0001", "Undefined identificator", elempos.GetLine(), elempos.GetColumn());
		errlist->Push_back(err);				
		result = false;
	}

	Pos EoFpos = elempos;
	Token EoF = Token("EOF", Token::Type::Terminal, EoFpos.GetLine() + 1, EoFpos.GetColumn());
	tokens->Push_back(EoF);
}

void Lexer::Id(std::string word, Pos elempos) {	
	ID tmp2 = ID(word, ID::Type::Not_defined, elempos.GetLine(), elempos.GetColumn());
	int idx = ids->Find(tmp2);
	if (idx < 0) {
		idx = ids->Length();
		ids->Push_back(tmp2);
	}

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

void Lexer::Constant(std::string word, Pos elempos, bool& result) {
	int constant = 0;
	if (word[0] != '$' && word[1] != '$') {
		for (size_t idx = 0; idx < word.length(); ++idx) {
			if (idx == 0 && word[idx] == '-')
				continue;

			if (word[idx] >= '0' && word[idx] <= '9')
				constant = constant * 10 + word[idx] - '0';
			else { // Если неправильно было введена константа
				Error err("L0002", "Wrong symbol in dec integer constant", elempos.GetLine(), elempos.GetColumn());
				errlist->Push_back(err);
				result = false;
				return;
			}
		}

		if (word[0] == '-')
			constant *= -1;
	}
	else {
		bool isNegative = false;
		bool isNegative2 = false;
		//std::cout << "word " << word << std::endl;
		//std::cout << "wordlenght " << word.length() << std::endl;
		if (word[0] == '-') {
			word.erase(word.begin());
			isNegative2 = true;
		}
		if (word.length() < 5) {
			word[0] = '0';

			while (word.length() < 5) {
				word.insert(0, "0");
			}
		}
		//std::cout << "wordafter " << word << std::endl;

		for (size_t idx = 1; idx < word.length(); ++idx) {
			if (idx == 1 && (word[idx] == '8' || word[idx] == '9' || word[idx] >= 'a' && word[idx] <= 'f')) {
				isNegative = true;
				constant = constant * 16 + ((word[idx] >= '0' && word[idx] <= '9') ? word[idx] - '0' - 8 : word[idx] - 'a' + 2);
				continue;
			}

			if (word[idx] >= '0' && word[idx] <= '9' || word[idx] >= 'a' && word[idx] <= 'f')
				constant = constant * 16 + ((word[idx] >= '0' && word[idx] <= '9') ? word[idx] - '0' : word[idx] - 'a' + 10);
			else { // Если неправильно было введена константа
				Error err("L0002", "Wrong symbol in hex integer constant", elempos.GetLine(), elempos.GetColumn());
				errlist->Push_back(err);
				result = false;
				return;
			}
		}
		//std::cout << "cs " << constant << std::endl;
		if (isNegative)
			constant -= 32768;
		//std::cout << "-cs " << constant << std::endl;
		if (isNegative2)
			constant *= -1;
		//std::cout << "-$cs " << constant << std::endl;
	}

	if (constant < -32768 || constant > 32767) { // Если превысели допустимое значение 2 байт 
		Error err("L0003", "Integer constant must be between -32768 and 32767", elempos.GetLine(), elempos.GetColumn());
		errlist->Push_back(err);
		result = false;
	}
	else {
		Token* tmp = new Token(std::to_string(constant), Token::Type::Const, elempos.GetLine(), elempos.GetColumn());
		tokens->Push_back(*tmp);
		std::cout << "Token " << tmp->GetContent() << std::endl;
		delete tmp;
	}
}

void Lexer::IDList(Pos elempos, List<Token>* tlptr) {
	bool Procedure;
	int start = 0, stop = 0;
	int NumberArea;
	for (int i = 0; i < tlptr->Length(); i++) {

	}
}