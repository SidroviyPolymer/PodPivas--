#include "Token.h"

Token::Token() {}

Token::Token(std::string content, Type type) {
	this->content = content;
	this->type = type;
}

Token::Token(std::string content, Type type, int Line, int Column) {
	this->content = content;
	this->type = type;
	this->Line = Line;
	this->Column = Column;
}

Token::Type Token::GetType() { return type; }

std::string& Token::GetContent() { return content; }

std::ostream& operator<<(std::ostream& os, const Token& data) {
	os << "<" << data.content << ", " << data.type << " Строка/столбец: " << data.Line << ", " << data.Column << ">>";
	return os;
}
