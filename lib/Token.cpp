#include "Token.h"

Token::Token() {}

Token::Token(std::string content, Type type) {
	this->content = content;
	this->type = type;
}

Token::Type Token::GetType() { return type; }

std::string& Token::GetContent() { return content; }

std::ostream& operator<<(std::ostream& os, const Token& data) {
	os << "<" << data.content << ", " << data.type << ">";
	return os;
}
