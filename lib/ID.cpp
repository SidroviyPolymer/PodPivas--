#include "ID.hpp"

ID::ID() {}

ID::ID(std::string content, Type type) {
	this->content = content;
	this->type = type;
}

ID::ID(std::string content, Type type, int Line, int Column) {
	this->content = content;
	this->type = type;
	this->Line = Line;
	this->Column = Column;
}

ID::Type ID::GetType() { return type; }

std::string& ID::GetContent() { return content; }

bool ID::operator==(ID const& id) {
	return id.content == content;
}

std::ostream& operator<<(std::ostream& os, const ID& data) {
	os << "<" << data.content << ", " << data.type << ", Position: " << data.Line << ", " << data.Column << ">";
	return os;
}