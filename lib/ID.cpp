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
void ID::SetType(Type type) { this->type = type; }

void ID::SetVal(int val) { this->val = std::to_string(val); }

std::string ID::GetVal() { return val; }

std::string ID::GetArea() { return area; }
void ID::SetArea(std::string area) { this->area = area; }

std::string& ID::GetContent() { return content; }

std::pair<int, int> ID::GetPos() { return std::pair<int, int>(line, column); }
void ID::SetPos(int line, int column) { 
	this->line = line;
	this->column = column;
}

bool ID::operator==(ID const& id) {
	return (id.content == content) && (id.area == area);
}

std::ostream& operator<<(std::ostream& os, const ID& data) {
	os << "<" << data.content << ", " << data.type << ", Position: " << data.line << ", " << data.column << ", Area: " << data.area << ">";
	return os;
}