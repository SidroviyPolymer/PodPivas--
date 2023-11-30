#pragma once
#include <iostream>
#include <string>

class ID {
public:
	enum Type {
		Var,
		Const,
		Proc,
		Prog,
		Not_defined
	};

private:
	std::string content;
	Type type;
	int line;
	int column;
	std::string area;

public:
	ID();
	ID(std::string, Type);
	ID(std::string, Type, int, int);

	Type GetType();
	void SetType(Type);

	std::string GetArea();
	void SetArea(std::string);

	std::string& GetContent();

	std::pair<int, int> GetPos();
	void SetPos(int, int);

	friend std::ostream& operator<<(std::ostream&, const ID&);
	bool operator==(ID const&);
};