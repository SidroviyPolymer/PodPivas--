#include "Lexer.h"


Lexer::Lexer() { // конструктор класса ЛА, создаёт два списка
	flow = new List<std::string>();
	flow2 = new List<Pos>();
}

bool Lexer::Process(List<Token>* tlptr, List<std::string>* ilptr, std::string src) { // Запуск ЛА, на входе два списка и файл с кодом
	tokens = tlptr;
	ids = ilptr;
	bool result = true; // Результат работы ЛА

	if (!OpenFile(src)) // Не получилось открыть
		return false;

	Parse(); // Вызов парсера
	TokenList(result);

	std::cout << "result = " << result;
	if (result == true) {
		return true;
	}
	else {
		return result;
	}
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

		if (tmp == ':' || tmp == ',' || tmp == ';' || tmp == '(' || tmp == ')') { // Если встретили терминальный символ,а не коммент
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

		if (!isBegin && prev == '(') { // Если прошлый символ  '(' не часть коммента, то считаем его отдельным токеном
			flow->Push_back(word);
			Pos* buf2 = new Pos(line, column-1);
			flow2->Push_back(*buf2);
			word = "";
		}

		if (!isBegin && prev == ':') {
			if (tmp == '=') { // Если нынешний символ  '=' и образует ":=" то это целый токен, помещаем его
				word += '=';
				flow->Push_back(word);
				prev = tmp;
				word = "";
				continue;
			}
			else { // Если нынешний символ не '=', то считаем ':' отдельным токеном
				flow->Push_back(word);
				word = ""; 
			}
		}

		if (tmp == '\n' || tmp == ' ' || tmp == '\t') { //Если слово закончилось, то помещаем в предварительный список токенов и позиций
			if (word != "") { // Если не пустое
				flow->Push_back(word);
				Pos* buf = new Pos(line, column - word.length());
				flow2->Push_back(*buf);
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

void Lexer::TokenList(bool& result) { // Функция заполнение листов
	for (size_t idx = 0; idx < flow->Length(); ++idx) {
		std::string elem = flow->At(idx);
		Pos elempos = flow2->At(idx);
		if (terminals.Contains(elem)) { // Если есть совпадения с терминальными символами
			Terminal(elem,elempos);
			continue;
		}			
		if (elem[0] >= 'a' && elem[0] <= 'z' || elem[0] == '_') { // Если начинается с символа от a до z или _
			Id(elem,elempos);
			continue;
		}
		if (operations.Contains(elem)) { // Если есть совпадения с оператарами
			Operation(elem, elempos);
			continue;
		}
		if (elem[0] >= '0' && elem[0] <= '9' || elem[0] == '-' || elem[0] == '$') { // Если начинается на цифру, знак
			Constant(elem, elempos,result);
			continue;
		}

		else { // Если нет совпадений, то ошибка
			std::cout << "WRONG WORD Line : " << elempos.GetLine() << " Column : " << elempos.GetColumn() << std::endl;
			result = false;
		}
	}
}

void Lexer::Id(std::string word, Pos elempos) {
	size_t idx = 0;
	if (!ids->Contains(word)) { // Проверяем есть ли такой идентификатор уже в списке
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
			else { // Если неправильно было введена константа
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
			else { // Если неправильно было введена константа
				std::cout << "ERROR CONSTANT Line:" << elempos.GetLine() << " Column: " << elempos.GetColumn() << std::endl;
				result = false;
			}
		}

		if (isNegative)
			constant -= 32768;
	}

	if (constant < -32768 || constant > 32767) { // Если превысели допустимое значение 2 байт 
		std::cout << "OUT OF BOUNDS Line:" << elempos.GetLine() << " Column: " << elempos.GetColumn() << std::endl;
		result = false;
	}
	else {
		Token* tmp = new Token(std::to_string(constant), Token::Type::Const, elempos.GetLine(), elempos.GetColumn());
		tokens->Push_back(*tmp);
	}		
}