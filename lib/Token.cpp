#include "Token.h"

Token::Token() {}

Token::Token(std::string content, Type type) {
	this->content = content;
	this->type = type;
}

Token::Token(std::string content, Type type, int Line, int Column) {
	this->content = content;
	this->type = type;
	this->line = Line;
	this->column = Column;
}

Token::Type Token::GetType() { return type; }

std::string& Token::GetContent() { return content; }

std::pair<int, int> Token::GetPos() { return std::pair<int, int>(line, column); }

std::ostream& operator<<(std::ostream& os, const Token& data) {
	os << "<" << data.content << ", " << data.type << ", Position: " << data.line << ", " << data.column << ">";
	return os;
}
