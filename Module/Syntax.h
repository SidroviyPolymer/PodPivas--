#pragma once

#include "../lib/List.hpp"
#include "../lib/Token.h"
#include "../lib/Tree.hpp"
#include "../lib/Error.h"
#include "../lib/ID.h";

class Syntax {
private:


	List<Token>* tokens;
	List<std::string>* ids;
public:
	Syntax(List<Token>* tokens, List<std::string>* ids, List<Error>* errlist);

	bool Process();
};