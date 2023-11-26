#pragma once

#include "../lib/List.hpp"
#include "../lib/Token.h"
#include "../lib/Tree.hpp"
#include "../lib/Error.h"

class Syntax {
private:

public:
	Syntax();

	bool Process(List<Token>* tokens, List<std::string>* ids, List<Error>* errlist);
};