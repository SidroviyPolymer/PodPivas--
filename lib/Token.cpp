#include "Token.h"

Token::Token() {}

Token::Type Token::GetType() { return type; }

std::string& Token::GetContent() { return content; }
