#include "ID.h"

ID::ID() {}

ID::ID(std::string content, Type type) {
	this->content = content;
	this->type = type;
}

ID::ID(std::string content, Type type, int Line, int Column) {
	this->content = content;
	this->type = type;
	this->line = Line;
	this->column = Column;
}

ID::Type ID::GetType() { return type; }

std::string& ID::GetContent() { return content; }

bool ID::operator==(ID const& id) {
	return id.content == content;
}

std::ostream& operator<<(std::ostream& os, const ID& data) {
	os << "<" << data.content << ", " << data.type << ", Position: " << data.line << ", " << data.column << ">";
	return os;
}