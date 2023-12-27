#include "Syntax.h"

Syntax::Syntax(List<Token>* tokens, List<ID>* ids, List<Error>* errlist) {
	this->tokens = tokens;
	this->ids = ids;
	this->errlist = errlist;
}

bool Syntax::Process() {
	//<program>
	Program();

	return isGood;
}

Tree* Syntax::GetTree() {
	return syntaxTree;
}

void Syntax::Program() {
	if (tokens->Length() == 0)
		return;

	//program
	Token program = tokens->At(0);

	if (program.GetContent() != "program") {
		Token name("id" + std::to_string(ids->Length()), Token::Type::Id, 0, 0);
		Token semicolon(";", Token::Type::Terminal, 0, 0);
		ID progName("BEGIN", ID::Type::Not_defined, 0, 0);
		ids->Push_back(progName);
		tokens->Push_front(semicolon);
		tokens->Push_front(name);
	}
	else
		tokens->Pop_front();

	//<name>
	Token name = tokens->At(0);
	if (!DefineName(name, ID::Type::Prog, "global")) {
		//Œ¯Ë·Í‡
	}
	tokens->Pop_front();

	syntaxTree = new Tree(name.GetContent());

	//;
	Token semicolon = tokens->At(0);
	if (!Semicolon(semicolon)) {
		//Œ¯Ë·Í‡
	}
	tokens->Pop_front();

	//<block>		
	if (!Block(syntaxTree, "OP", "global")) {
		//Œ¯Ë·Í‡
	}

	//.
	Token point = tokens->At(0);
	if (point.GetContent() != "point") {
		//Œ¯Ë·Í‡;
	}
	tokens->Pop_front();
}

bool Syntax::DefineName(Token& name, ID::Type type, std::string area) {
	if (name.GetType() != Token::Type::Id)
		return false;

	size_t idx = std::stoi(name.GetContent().substr(2));
	ID& nameID = ids->At(idx);

	if (nameID.GetArea() != "undefined") {
		//std::cout << nameID.GetContent() << " " << nameID.GetArea() << " " << area << std::endl;
		if (nameID.GetArea() == area) {
			//Œÿ»¡ ¿: œÓ‚ÚÓÌÓÂ Ó·˙ˇ‚ÎÂÌËÂ
			Error err = Error("S", "Variable " + nameID.GetContent() + " is already declared.", name.GetPos().first, name.GetPos().second);
			errlist->Push_back(err);
			isGood = false;
			return false;
		}

		ID cpyID = nameID;
		cpyID.SetArea(area);
		if (ids->Find(cpyID) > 0) {
			//Œÿ»¡ ¿: œÓ‚ÚÓÌÓÂ Ó·˙ˇ‚ÎÂÌËÂ
			Error err = Error("S", "Variable " + nameID.GetContent() + " is already declared.", name.GetPos().first, name.GetPos().second);
			errlist->Push_back(err);
			isGood = false;
			return false;
		}

		ID newNameID = ID(nameID.GetContent(), type, name.GetPos().first, name.GetPos().second);
		newNameID.SetArea(area);
		name.GetContent() = "id" + std::to_string(ids->Length());
		ids->Push_back(newNameID);
		return true;
	}
	
	nameID.SetType(type);
	nameID.SetArea(area);

	return true;
}

bool Syntax::DefineProcName(Token& name, std::string area) {
	if (name.GetType() != Token::Type::Id)
		return false;

	size_t idx = std::stoi(name.GetContent().substr(2));
	ID& nameID = ids->At(idx);
	std::string expectedName = "";	
	std::string currentName = nameID.GetContent();

	if (currentName[0] == '@') {
		expectedName = currentName.substr(1);
		size_t length = 0;
		while (expectedName[0] >= '0' && expectedName[0] <= '9') {
			length = length * 10 + (expectedName[0] - '0');
			expectedName = expectedName.substr(1);
		}

		expectedName = expectedName.substr(0, length);
		nameID.GetContent() = expectedName;
		//std::cout << currentName << " " << expectedName << std::endl;
	}
	
	bool result = DefineName(name, ID::Type::Proc, area);
	nameID.GetContent() = currentName;

	return result;
}

bool Syntax::Semicolon(Token semicolon) {
	return semicolon.GetContent() == ";";
}

bool Syntax::Block(Tree* tree, std::string label, std::string area) {
	if (tokens->Length() == 0)		
		return false;

	Tree* definitions = tree->CreateLeft();
	Tree* operators = tree->CreateRight();

	//<constant_section>
	if (ConstantSection(definitions, area))
		definitions = definitions->CreateRight();

	//tree->Print();

	//<variables_section>
	if (VariableSection(definitions, area))
		definitions = definitions->CreateRight();

	//tree->Print();

	//<procedures_section>
	ProcedureSection(definitions, area);


	//<operators_section>
	if (!OperatorsSection(operators, area, label, 1)) {
		//Œ¯Ë·Í‡
		isGood = false;
		return false;
	}

	return true;
}

bool Syntax::ConstantSection(Tree* tree, std::string area) {
	if (tokens->Length() == 0)
		return false;

	//const
	Token _const = tokens->At(0);
	if (_const.GetContent() != "const")
		return false;

	tokens->Pop_front();

	tree->SetData("const");	
	tree = tree->CreateLeft();

	//<definition_constant>
	if (!DefinitionConstant(tree, area)) {
		//Œ¯Ë·Í‡
		isGood = false;
		return false;
	}	
	
	Tree* prev = tree;
	tree = tree->CreateRight();

	//[{<definition_constant>}]
	while (DefinitionConstant(tree, area)) {
		prev = tree;
		tree = tree->CreateRight();
	}

	prev->DeleteRight();
	return true;
}

bool Syntax::DefinitionConstant(Tree* tree, std::string area) {
	if (tokens->Length() == 0)
		return false;

	//<name>
	Token name = tokens->At(0);
	if (!DefineName(name, ID::Type::Const, area))
		return false;

	tokens->Pop_front();

	tree->SetData(name.GetContent());

	//=
	Token equal = tokens->At(0);
	if (equal.GetContent() != "=") {
		//Œÿ»¡ ¿
		isGood = false;
		return false;
	}
	tokens->Pop_front();

	//<constant_expression>	
	int val = 0; 
	if (!ConstantExpression(tree->CreateLeft(), area, val)) {
		//Œÿ»¡ ¿
		isGood = false;
		return false;
	}

	size_t idx = std::stoi(name.GetContent().substr(2));
	ids->At(idx).SetVal(val);

	//;
	Token semicolon = tokens->At(0);
	if (!Semicolon(semicolon)) {
		//Œ¯Ë·Í‡
		isGood = false;
		return false;
	}
	tokens->Pop_front();

	return true;
}

bool Syntax::ConstantExpression(Tree* tree, std::string area, int& res) {
	if (tokens->Length() == 0)
		return false;

	List<Token>* postfix = new List<Token>();
	if (!GetConstPostfix(postfix, area))
		return false;

	List<int>* stack = new List<int>();

	while (postfix->Length() != 0) {
		Token token = postfix->Pop_front();

		if (token.GetType() == Token::Type::Const) {
			int value = std::stoi(token.GetContent());
			stack->Push_front(value);
			continue;
		}

		if (token.GetType() == Token::Type::Id && Constant(token)) {
			size_t idx = std::stoi(token.GetContent().substr(2));
			ID id = ids->At(idx);
			std::string val = id.GetVal();
			if (val == "undefined") {
				Error err = Error("S0010", "Constant must be initialized", token.GetPos().first, token.GetPos().second);
				errlist->Push_back(err);
				return false;
			}
			int value = std::stoi(val);
			stack->Push_front(value);
			continue;
		}

		if (token.GetType() == Token::Type::Operation) {
			std::string oper = token.GetContent();
			int a = stack->Pop_front();
			int b = stack->Pop_front();
			if (oper == "+") {
				int res = a + b;
				stack->Push_front(res);
			}
			else if (oper == "-") {
				int res = a - b;
				stack->Push_front(res);
			}
			else if (oper == "*") {
				int res = a * b;
				stack->Push_front(res);
			}
			else if (oper == "div") {
				int res = a / b;
				stack->Push_front(res);
			}
			else if (oper == "mod") {
				int res = a % b;
				stack->Push_front(res);
			}

			continue;
		}

		return false;
	}
	
	res = stack->Pop_front();
	tree->SetData(std::to_string(res));

	delete stack;
	delete postfix;

	return true;
}

bool Syntax::GetConstPostfix(List<Token>* result, std::string area) {
	List<Token> stack;

	bool exprExpected = true;
	bool isNegative = false;

	while (true) {
		if (tokens->Length() == 0)
			return false;

		Token token = tokens->At(0);

		if (token.GetContent() == "(") {
			if (!exprExpected) {
				//Œ¯Ë·Í‡
				return false;
			}

			stack.Push_front(token);
			tokens->Pop_front();
			exprExpected = true;
			continue;
		}

		if (token.GetContent() == "+" || token.GetContent() == "-") {
			if (exprExpected) {
				if (token.GetContent() == "-" && Constant(token)) {
					Token tmp = Token("-1", Token::Type::Const);
					result->Push_back(tmp);
					tokens->Pop_front();
					exprExpected = true;
					isNegative = true;
					continue;
				}
				//Œ¯Ë·Í‡
				isGood = false;
				return false;
			}

			if (isNegative) {
				Token tmp = Token("*", Token::Type::Operation);
				result->Push_back(tmp);
				isNegative = false;
			}

			if (stack.Length() != 0) {
				Token tmp = stack.At(0);
				while (tmp.GetContent() != "(") {
					tmp = stack.Pop_front();
					result->Push_back(tmp);
					if (stack.Length() == 0)
						break;
					tmp = stack.At(0);
				}
			}
			stack.Push_front(token);
			tokens->Pop_front();
			exprExpected = true;
			continue;
		}

		if (token.GetContent() == "*" || token.GetContent() == "div") {
			if (exprExpected) {
				//Œ¯Ë·Í‡
				isGood = false;
				return false;
			}

			if (isNegative) {
				Token tmp = Token("*", Token::Type::Operation);
				result->Push_back(tmp);
				isNegative = false;
			}

			if (stack.Length() != 0) {
				Token tmp = stack.At(0);
				while (tmp.GetContent() != "(" && tmp.GetContent() != "+" && tmp.GetContent() != "-") {
					tmp = stack.Pop_front();
					result->Push_back(tmp);
					if (stack.Length() == 0)
						break;
					tmp = stack.At(0);
				}
			}
			stack.Push_front(token);
			tokens->Pop_front();
			exprExpected = true;
			continue;
		}

		if (token.GetContent() == ")") {
			if (exprExpected) {
				//Œ¯Ë·Í‡
				isGood = false;
				return false;
			}

			if (isNegative) {
				Token tmp = Token("*", Token::Type::Operation);
				result->Push_back(tmp);
				isNegative = false;
			}

			if (stack.Length() != 0) {
				Token tmp = stack.At(0);
				if (tmp.GetContent() == "(") {
					//Œ¯Ë·Í‡
					isGood = false;
					return false;
				}
				while (tmp.GetContent() != "(") {
					tmp = stack.Pop_front();
					result->Push_back(tmp);
					if (stack.Length() == 0 && tmp.GetContent() != ")") {
						//Œ¯Ë·Í‡
						isGood = false;
						return false;
					}
					tmp = stack.At(0);
				}
			}
			stack.Pop_front();
			tokens->Pop_front();
			continue;
		}

		if (Constant(token)) {
			if (!exprExpected) {
				//Œ¯Ë·Í‡
				isGood = false;
				return false;
			}

			if (token.GetType() == Token::Type::Id && !CheckArea(token, area))
				return false;

			result->Push_back(token);
			tokens->Pop_front();
			exprExpected = false;
			continue;
		}

		if (token.GetContent() == ";") {
			while (stack.Length() != 0) {
				Token tmp = stack.Pop_front();
				if (tmp.GetContent() == "(") {
					//Œ¯Ë·Í‡
					isGood = false;
					return false;
				}

				result->Push_back(tmp);
			}

			if (isNegative) {
				Token tmp = Token("*", Token::Type::Operation);
				result->Push_back(tmp);
				isNegative = false;
			}

			if (result->Length() == 0) {
				//Œ¯Ë·Í‡
				isGood = false;
				return false;
			}

			return true;
		}

		//Œ¯Ë·Í‡
		isGood = false;
		return false;
	}
}

bool Syntax::Constant(Token constant) {
	//<integer>
	if (constant.GetType() == Token::Type::Const) {
		return true;
	}
	//<constant_name>
	if (constant.GetType() == Token::Type::Id) {
		size_t idx = std::stoi(constant.GetContent().substr(2));
		ID& id = ids->At(idx);
		return id.GetType() == ID::Type::Const;
	}
	//<sign><constant_name>
	if (constant.GetContent() == "-" || constant.GetContent() == "+") {
		return Constant(tokens->At(1));
	}

	return false;
}

bool Syntax::VariableSection(Tree* tree, std::string area) {
	if (tokens->Length() == 0)
		return false;

	//var
	Token var = tokens->At(0);
	if (var.GetContent() != "var")
		return false;
	tokens->Pop_front();

	tree->SetData("var");
	Tree* DStree = tree->CreateLeft();

	//<description_similar_var>
	size_t DSidx = 0;
	if (!DescriptionSimilarVar(DStree, area, DSidx++)) {
		//Œ¯Ë·Í‡
		isGood = false;
		return false;
	}

	//;
	Token semicolon = tokens->At(0);
	if (!Semicolon(semicolon)) {
		//Œ¯Ë·Í‡
		isGood = false;
		return false;
	}
	tokens->Pop_front();

	//[<description_similar_var>]
	Tree* prev = DStree;
	DStree = DStree->CreateRight();
	while (DescriptionSimilarVar(DStree, area, DSidx++)) {
		//;
		Token semicolon = tokens->At(0);
		if (!Semicolon(semicolon)) {
			//Œ¯Ë·Í‡
			isGood = false;
			return false;
		}
		tokens->Pop_front();
		prev = DStree;
		DStree = DStree->CreateRight();
	}
	prev->DeleteRight();

	return true;
}

bool Syntax::DescriptionSimilarVar(Tree* tree, std::string area, size_t idx) {
	if (tokens->Length() == 0)
		return false;	

	//std::cout << "DS" + std::to_string(idx) << std::endl;
	tree->SetData("DS" + std::to_string(idx));
	tree = tree->CreateLeft();

	//name
	Token name = tokens->At(0);
	if (!DefineName(name, ID::Type::Var, area))
		return false;

	tokens->Pop_front();
	tree->SetData(name.GetContent());

	//[{, name}]
	Token comma = tokens->At(0);
	Tree* prev = tree;
	tree = tree->CreateRight();
	while (comma.GetContent() == ",") {
		tokens->Pop_front();
		Token name = tokens->At(0);
		if (!DefineName(name, ID::Type::Var, area)) {
			//Œ¯Ë·Í‡
			isGood = false;
			return false;
		}
		tokens->Pop_front();
		tree->SetData(name.GetContent());
		
		comma = tokens->At(0);
		prev = tree;
		tree = tree->CreateRight();
	}
	prev->DeleteRight();

	//:
	Token colon = tokens->At(0);
	if (colon.GetContent() != ":") {
		//Œ¯Ë·Í‡
		isGood = false;
		return false;
	}
	tokens->Pop_front();

	//integer
	Token integer = tokens->At(0);
	if (integer.GetContent() != "integer") {
		//Œ¯Ë·Í‡
		isGood = false;
		return false;
	}
	tokens->Pop_front();

	return true;
}

bool Syntax::ProcedureSection(Tree* tree, std::string area) {
	if (tokens->Length() == 0)
		return false;

	size_t procIdx = 0;

	Token procedure = tokens->At(0);
	if (procedure.GetContent() != "procedure")
		return false;

	tree->SetData("procedure");
	tree = tree->CreateLeft();

	while (procedure.GetContent() == "procedure") {
		std::string _area = (area == "global" ? "" : (area + "_")) + "PROC" + std::to_string(procIdx++);

		tokens->Pop_front();

		tree->SetData("PR" + std::to_string(procIdx - 1));
		Tree* procTree = tree->CreateLeft();

		//id
		Token name = tokens->At(0);
		if (!DefineProcName(name, area))
			return false;

		tokens->Pop_front();
		procTree->SetData(name.GetContent());

		//(
		std::string parStr = "";
		size_t parCount = 0;
		Token bracket = tokens->At(0);
		if (bracket.GetContent() == "(") {
			tokens->Pop_front();
			//[param]			
			ParamSection(procTree->CreateLeft(), parStr, parCount, _area);

			//)
			bracket = tokens->At(0);
			if (bracket.GetContent() != ")") {
				//Œ¯Ë·Í‡
				isGood = false;
				return false;
			}
			tokens->Pop_front();
		}		

		//std::cout << parCount << parStr << std::endl;
		size_t idx = std::stoi(name.GetContent().substr(2));
		ID& id = ids->At(idx);
		std::string newProcName = "@" + std::to_string(id.GetContent().length()) + id.GetContent() + "_" + std::to_string(parCount) + parStr;
		id.GetContent() = newProcName;

		if (!CheckProcName(id, idx)) {
			//Œ¯Ë·Í‡
			Error err = Error("S", "This procedure is already geclared", name.GetPos().first, name.GetPos().second);
			errlist->Push_back(err);
			isGood = false;
			return true;
		}

		//;
		Token semicolon = tokens->At(0);
		if (!Semicolon(semicolon)) {
			//Œ¯Ë·Í‡
			isGood = false;
			return false;
		}			
		tokens->Pop_front();

		//block      label pr[idx]_op or		
		std::string nodeName = "block";

		if (!Block(procTree->CreateRight(nodeName), _area + "_OP", _area)) {
			//Œ¯Ë·Í‡
			isGood = false;
			return false;
		}

		//;
		semicolon = tokens->At(0);
		if (!Semicolon(semicolon)) {
			//Œ¯Ë·Í‡
			isGood = false;
			return false;
		}
		tokens->Pop_front();

		procedure = tokens->At(0);
		if (procedure.GetContent() == "procedure")
			tree = tree->CreateRight();		
	}	

	return true;
}

bool Syntax::ParamSection(Tree* tree, std::string& parStr, size_t& parCount, std::string area) {
	//<description>
	if (!ParamDescription(tree, parStr, parCount, area))
		return false;

	//[; <description>]
	Token semicolon = tokens->At(0);
	while (Semicolon(semicolon)) {
		tokens->Pop_front();
		tree = tree->CreateRight();
		if (!ParamDescription(tree, parStr, parCount, area))
			return false;

		semicolon = tokens->At(0);
	}

	return true;
}

bool Syntax::ParamDescription(Tree* tree, std::string& parStr, size_t& parCount, std::string area) {
	//[(var, const)]
	Token var = tokens->At(0);
	ID::Type type = ID::Type::Var;
	char ctype = 'i';
	if (var.GetContent() == "var") {
		tokens->Pop_front();
		tree->SetData("var-param");		
		ctype = 'v';
	}		
	else if (var.GetContent() == "const") {
		tokens->Pop_front();
		tree->SetData("const-param");
		type = ID::Type::Const;
		ctype = 'c';
	}		
	else
		tree->SetData("param");

	tree = tree->CreateLeft();

	//name	
	Token name = tokens->At(0);

	if (name.GetType() != Token::Type::Id)
		return false;

	if (!DefineName(name, type, area)) {
		//Œ¯Ë·Í‡
		isGood = false;
		return false;
	}		

	tokens->Pop_front();
	tree->SetData(name.GetContent());
	parStr += ctype;
	++parCount;

	//[, name]
	Token comma = tokens->At(0);
	while (comma.GetContent() == ",") {
		tokens->Pop_front();
		name = tokens->At(0);
		tree = tree->CreateRight();

		if (name.GetType() != Token::Type::Id) {
			//Œ¯Ë·Í‡
			isGood = false;
			return false;
		}			

		if (!DefineName(name, type, area)) {
			//Œ¯Ë·Í‡
			isGood = false;
			return false;
		}

		tokens->Pop_front();
		tree->SetData(name.GetContent());
		parStr += ctype;
		++parCount;
		comma = tokens->At(0);
	}	

	//:
	Token colon = tokens->At(0);
	if (colon.GetContent() != ":") {
		//Œ¯Ë·Í‡
		isGood = false;
		return false;
	}
	tokens->Pop_front();

	Token integer = tokens->At(0);
	if (integer.GetContent() != "integer") {
		//Œ¯Ë·Í‡
		isGood = false;
		return false;
	}
	tokens->Pop_front();
	//integer
	return true;
}

bool Syntax::isProcedure(Token token) {
	if (token.GetType() == Token::Type::Id) {
		size_t idx = std::stoi(token.GetContent().substr(2));
		ID id = ids->At(idx);

		return id.GetType() == ID::Type::Proc;
	}
	return false;
}

bool Syntax::CheckProcName(ID& id, size_t idx) {
	size_t fidx = ids->Find(id);
	return fidx == idx;
}

bool Syntax::OperatorsSection(Tree* tree, std::string area, std::string label, size_t idx) {
	//<compound operator>
	return CompoundOperator(tree, area, label, idx);	
}

bool Syntax::CompoundOperator(Tree* tree, std::string area, std::string label, size_t idx) {
	if (tokens->Length() == 0)
		return false;

	//begin	
	Token _begin = tokens->At(0);
	if (_begin.GetContent() != "begin")
		return false;
	
	tokens->Pop_front();

	//operator
	if (Operator(tree, area, label, idx)) {
		tree = tree->CreateRight();
		++idx;
	}		

	//[{; <operator>}]	
	Token semicolon = tokens->At(0);	
	while (semicolon.GetContent() == ";") {
		tokens->Pop_front();		
		if (Operator(tree, area, label, idx)) {
			tree = tree->CreateRight();
			++idx;
		}			
		semicolon = tokens->At(0);
	}

	//null-operator
	NULLOP(tree, label, idx);

	//end
	Token end = tokens->At(0);
	if (end.GetContent() != "end") {
		//Œ¯Ë·Í‡
		isGood = false;
		return false;
	}
	tokens->Pop_front();

	return true;
}

bool Syntax::Operator(Tree* tree, std::string area, std::string label, size_t idx) {
	//<simple operator>
	if (SimpleOperator(tree, area, label, idx))
		return true;

	//<complex operator>	
	if (ComplexOperator(tree, area, label, idx))
		return true;

	return false;
}

bool Syntax::SimpleOperator(Tree* tree, std::string area, std::string label, size_t idx) {
	//<assignment_operator>
	if (AssigmentOperator(tree, area, label, idx))
		return true;

	//<procedure_operator>
	if (ProcedureOperator(tree, area, label, idx))
		return true;

	//<exit_operator>
	if (ExitOperator(tree, label, idx))
		return true;

	//<null_operator>
	if (Null(tree, label, idx))
		return true;

	return false;
}

bool Syntax::AssigmentOperator(Tree* tree, std::string area, std::string label, size_t idx) {
	//<variable>
	Token var = tokens->At(0);

	if (var.GetType() != Token::Type::Id)
		return false;

	if (!CheckArea(var, area))
		return false;	

	//:=
	Token assign = tokens->At(1);
	if (assign.GetContent() != ":=")
		return false;
	
	if (Constant(var)) {
		Error err = Error("S", var.GetContent() + " is a constant identifier. Variable identifier expected", var.GetPos().first, var.GetPos().second);
		errlist->Push_back(err);
		isGood = false;
		return false;
	}

	tokens->Pop_front();
	tokens->Pop_front();
	
	//<expression>
	Tree* exprTree = new Tree();
	if (!Expression(exprTree, area))
		return false;

	tree->SetData(label + std::to_string(idx));
	tree = tree->CreateLeft();
	tree->SetData(":=");
	tree->CreateLeft(var.GetContent());
	tree->CreateRight();
	tree->SetRight(exprTree);

	return true;
}

bool Syntax::Expression(Tree* tree, std::string area) {
	if (tokens->Length() == 0)
		return false;

	List<Token> postfix;

	if (!GetPostfix(&postfix, area))
		return false;

	//for (size_t idx = 0; idx < postfix.Length(); ++idx)
	//	std::cout << postfix.At(idx).GetContent() << " ";
	//std::cout << std::endl;

	return GetExpressionTree(tree, &postfix);
}

bool Syntax::GetPostfix(List<Token>* result, std::string area) {
	List<Token> stack;

	bool exprExpected = true;
	bool isNegative = false;

	while (true) {
		if (tokens->Length() == 0)
			return false;

		Token token = tokens->At(0);

		if (token.GetContent() == "(") {
			if (!exprExpected) {
				Error err = Error("S", "Operator or ; expected but " + token.GetContent() + " found", token.GetPos().first, token.GetPos().second);
				errlist->Push_back(err);
				isGood = false;
				return false;
			}

			stack.Push_front(token);
			tokens->Pop_front();
			exprExpected = true;
			continue;
		}

		if (token.GetContent() == "+" || token.GetContent() == "-") {
			if (exprExpected) {
				if (token.GetContent() == "-" && Constant(token)) {
					Token tmp = Token("-1", Token::Type::Const);
					result->Push_back(tmp);
					tokens->Pop_front();
					exprExpected = true;
					isNegative = true;
					continue;
				}
				//Œ¯Ë·Í‡
				Error err = Error("S", "Expression expected but " + token.GetContent() + " found", token.GetPos().first, token.GetPos().second);
				errlist->Push_back(err);
				isGood = false;
				return false;
			}

			if (isNegative) {
				Token tmp = Token("*", Token::Type::Operation);
				result->Push_back(tmp);
				isNegative = false;
			}

			if (stack.Length() != 0) {
				Token tmp = stack.At(0);
				while (tmp.GetContent() != "(") {
					tmp = stack.Pop_front();
					result->Push_back(tmp);
					if (stack.Length() == 0)
						break;
					tmp = stack.At(0);
				}
			}
			stack.Push_front(token);
			tokens->Pop_front();
			exprExpected = true;
			continue;
		}

		if (token.GetContent() == "*" || token.GetContent() == "div") {
			if (exprExpected) {
				//Œ¯Ë·Í‡
				Error err = Error("S", "Expression expected but " + token.GetContent() + " found", token.GetPos().first, token.GetPos().second);
				errlist->Push_back(err);
				isGood = false;
				return false;
			}

			if (isNegative) {
				Token tmp = Token("*", Token::Type::Operation);
				result->Push_back(tmp);
				isNegative = false;
			}

			if (stack.Length() != 0) {
				Token tmp = stack.At(0);
				while (tmp.GetContent() != "(" && tmp.GetContent() != "+" && tmp.GetContent() != "-") {
					tmp = stack.Pop_front();
					result->Push_back(tmp);
					if (stack.Length() == 0)
						break;
					tmp = stack.At(0);
				}
			}
			stack.Push_front(token);
			tokens->Pop_front();
			exprExpected = true;
			continue;
		}

		if (token.GetContent() == ")") {
			if (exprExpected) {
				//Œ¯Ë·Í‡
				Error err = Error("S", "Expression expected but " + token.GetContent() + " found", token.GetPos().first, token.GetPos().second);
				errlist->Push_back(err);
				isGood = false;
				return false;
			}

			if (isNegative) {
				Token tmp = Token("*", Token::Type::Operation);
				result->Push_back(tmp);
				isNegative = false;
			}

			if (stack.Length() != 0) {
				Token tmp = stack.At(0);
				while (tmp.GetContent() != "(") {
					tmp = stack.Pop_front();
					result->Push_back(tmp);
					if (stack.Length() == 0 && tmp.GetContent() != ")") {
						//Œ¯Ë·Í‡
						Error err = Error("S", "There is no ( for current )", token.GetPos().first, token.GetPos().second);
						errlist->Push_back(err);
						isGood = false;
						return false;
					}
					tmp = stack.At(0);
				}
			}
			stack.Pop_front();
			tokens->Pop_front();
			continue;
		}

		if (Constant(token) || isVar(token)) {
			if (!exprExpected) {
				//Œ¯Ë·Í‡
				Error err = Error("S", "Operator or ; expected but " + token.GetContent() + " found", token.GetPos().first, token.GetPos().second);
				errlist->Push_back(err);
				isGood = false;
				return false;
			}

			if (token.GetType() == Token::Type::Id && !CheckArea(token, area))
				return false;

			result->Push_back(token);
			tokens->Pop_front();
			exprExpected = false;
			continue;
		}

		if (token.GetContent() == ";") {
			while (stack.Length() != 0) {
				Token tmp = stack.Pop_front();
				if (tmp.GetContent() == "(") {
					//Œ¯Ë·Í‡
					Error err = Error("S", "There is no ) for current (", tmp.GetPos().first, tmp.GetPos().second);
					errlist->Push_back(err);
					isGood = false;
					return false;
				}

				result->Push_back(tmp);
			}

			if (isNegative) {
				Token tmp = Token("*", Token::Type::Operation);
				result->Push_back(tmp);
				isNegative = false;
			}

			if (result->Length() == 0) {
				//Œ¯Ë·Í‡
				Error err = Error("S", "Expression expected but " + token.GetContent() + " found", token.GetPos().first, token.GetPos().second);
				errlist->Push_back(err);
				isGood = false;
				return false;
			}

			return true;
		}

		//Œ¯Ë·Í‡
		isGood = false;

		if (!exprExpected) {
			Error err = Error("S", "Operator or ; expected but " + token.GetContent() + " found", token.GetPos().first, token.GetPos().second);
			errlist->Push_back(err);
			return false;
		}
		
		if (token.GetType() != Token::Type::Id) {
			Error err = Error("S", "Expression expected but " + token.GetContent() + " found", token.GetPos().first, token.GetPos().second);
			errlist->Push_back(err);
			return false;
		}		

		return CheckArea(token, area);
	}
}

bool Syntax::GetExpressionTree(Tree* tree, List<Token>* postfix) {	
	Token _first = postfix->Pop_back();

	if (isVar(_first) || Constant(_first)) {
		tree->SetData(_first.GetContent());
		return true;
	}

	if (_first.GetType() != Token::Type::Operation) {
		//Œ¯Ë·Í‡
		isGood = false;
		return false;
	}

	tree->SetData(_first.GetContent());

	Token right = postfix->Pop_back();

	if (right.GetType() == Token::Type::Operation) {
		postfix->Push_back(right);
		if (!GetExpressionTree(tree->CreateRight(), postfix))
			return false;
	}
	else
		tree->CreateRight(right.GetContent());

	Token left = postfix->Pop_back();

	if (left.GetType() == Token::Type::Operation) {
		postfix->Push_back(left);
		if (!GetExpressionTree(tree->CreateLeft(), postfix))
			return false;
	}
	else
		tree->CreateLeft(left.GetContent());

	return true;
}

bool Syntax::isVar(Token token) {
	if (token.GetType() == Token::Type::Id) {
		size_t idx = std::stoi(token.GetContent().substr(2));
		ID id = ids->At(idx);
		
		return id.GetType() == ID::Type::Var;
	}
	return false;
}

bool Syntax::ProcedureOperator(Tree* tree, std::string area, std::string label, size_t idx) {
	//<procedure name>	
	if (tokens->Length() == 0) {
		//Œ¯Ë·Í‡
		isGood = false;
		return false;
	}

	Token procName = tokens->At(0);

	if (!isProcedure(procName))
		return false;

	if (!CheckArea(procName, area))
		return false;

	tree->SetData(label + std::to_string(idx));
	tree = tree->CreateLeft();
	tree->SetData("CALL");
	Tree* nameTree = tree->CreateLeft();
	Tree* paramTree = tree->CreateRight();
	nameTree->SetData(procName.GetContent());

	tokens->Pop_front();

	//[param]

	size_t idIdx = std::stoi(procName.GetContent().substr(2));
	ID id = ids->At(idIdx);
	size_t lidx = id.GetContent().find_last_of("_");
	std::string paramStr = id.GetContent().substr(lidx + 1);
	size_t paramCount = 0;

	while (paramStr[0] >= '0' && paramStr[0] <= '9') {
		paramCount = paramCount * 10 + paramStr[0] - '0';
		paramStr = paramStr.substr(1);
	}

	Token bracket = tokens->At(0);
	if (bracket.GetContent() != "(") {
		if (paramCount == 0)
			return true;

		//Œ¯Ë·Í‡
		std::string expectedName = id.GetContent().substr(1);
		size_t length = 0;
		while (expectedName[0] >= '0' && expectedName[0] <= '9') {
			length = length * 10 + (expectedName[0] - '0');
			expectedName = expectedName.substr(1);
		}
		expectedName = expectedName.substr(0, length);

		Error err = Error("S", "Wrong number of parameters specified for call to \"" + expectedName + "\"", procName.GetPos().first, procName.GetPos().second);
		errlist->Push_back(err);
		isGood = false;
		return true;
	}
	tokens->Pop_front();

	for (size_t paramIdx = 0; paramIdx < paramCount; ++paramIdx) {
		char paramType = paramStr[0];
		paramStr = paramStr.substr(1);
		Token param = tokens->At(0);

		if (!CheckArea(param, area))
			return true;

		switch (paramType) {
		case 'i':
			if (!isVar(param) && !Constant(param)) {
				//Œ¯Ë·Í‡
				Error err = Error("S", "Illegal expression", param.GetPos().first, param.GetPos().second);
				errlist->Push_back(err);
				isGood = false;
				return true;
			}
			break;
		case 'v':
			if (!isVar(param)) {
				//Œ¯Ë·Í‡
				if (Constant(param)) {
					Error err = Error("S", "Variable identifier expected", param.GetPos().first, param.GetPos().second);
					errlist->Push_back(err);
				}
				else {
					Error err = Error("S", "Illegal expression", param.GetPos().first, param.GetPos().second);
					errlist->Push_back(err);
				}

				isGood = false;
				return true;
			}
			break;
		case 'c':
			if (!isVar(param) && !Constant(param)) {
				//Œ¯Ë·Í‡
				Error err = Error("S", "Illegal expression", param.GetPos().first, param.GetPos().second);
				errlist->Push_back(err);
				isGood = false;
				return true;
			}
			break;
		}
		paramTree->SetData(param.GetContent());		
		tokens->Pop_front();
		
		if (paramIdx == paramCount - 1)
			continue;

		paramTree = paramTree->CreateRight();

		Token comma = tokens->At(0);
		if (comma.GetContent() != ",") {
			std::string found = comma.GetContent();
			if (comma.GetType() == Token::Type::Id) {
				size_t idx = std::stoi(found.substr(2));
				found = ids->At(idx).GetContent();
			}

			//Œ¯Ë·Í‡
			Error err = Error("S", ", expected but " + found + " found", comma.GetPos().first, comma.GetPos().second);
			errlist->Push_back(err);
			isGood = false;
			return true;
		}
		tokens->Pop_front();
	}

	bracket = tokens->At(0);
	if (bracket.GetContent() != ")") {
		if (paramCount == 0)
			return true;

		//Œ¯Ë·Í‡
		std::string expectedName = id.GetContent().substr(1);
		size_t length = 0;
		while (expectedName[0] >= '0' && expectedName[0] <= '9') {
			length = length * 10 + (expectedName[0] - '0');
			expectedName = expectedName.substr(1);
		}
		expectedName = expectedName.substr(0, length);

		Error err = Error("S", ") expected but " + bracket.GetContent() + " found", bracket.GetPos().first, bracket.GetPos().second);
		errlist->Push_back(err);
		isGood = false;
		return true;
	}
	tokens->Pop_front();
}

bool Syntax::ComplexOperator(Tree* tree, std::string area, std::string label, size_t idx) {
	tree->SetData(label + std::to_string(idx));

	if (CompoundOperator(tree->CreateLeft(), area, label + std::to_string(idx) + "_", 1))
		return true;

	tree->SetLeft(nullptr);	

	return false;
}

bool Syntax::ExitOperator(Tree* tree, std::string label, size_t idx) {
	Token exit = tokens->At(0);
	if (exit.GetContent() != "exit") {
		return false;
	}
	tokens->Pop_front();

	tree->SetData(label + std::to_string(idx));
	tree = tree->CreateLeft();
	tree->SetData("exit");

	return true;
}

bool Syntax::Null(Tree* tree, std::string label, size_t idx) {
	Token token = tokens->At(0);
	if (token.GetContent() != "null")
		return false;

	tokens->Pop_front();
	NULLOP(tree, label, idx);
	return true;
}

void Syntax::NULLOP(Tree* tree, std::string label, size_t idx) {
	tree->SetData(label + std::to_string(idx));
	tree = tree->CreateLeft();
	tree->SetData("NULL");
}

bool Syntax::CheckArea(Token& token, std::string area) {
	//std::cout << token.GetContent() << " " << area << std::endl;

	size_t idx = std::stoi(token.GetContent().substr(2));
	ID id = ids->At(idx);

	if (id.GetArea() == area)
		return true;

	ID expected = id;
	expected.SetArea(area);

	int newIdx = ids->Find(expected);
	if (newIdx > 0) {
		token.GetContent() = "id" + std::to_string(newIdx);
		return true;
	}

	size_t lidx = area.find_last_of('_');	
	while (lidx != std::string::npos) {
		area = area.substr(0, lidx);
		expected.SetArea(area);
		
		int newIdx = ids->Find(expected);
		if (newIdx > 0) {
			token.GetContent() = "id" + std::to_string(newIdx);
			return true;
		}

		lidx = area.find_last_of('_');
	}

	if (id.GetArea() == "global")
		return true;

	//Œ¯Ë·Í‡
	Error err = Error("S", id.GetContent() + " is an undeclared identifier", token.GetPos().first, token.GetPos().second);
	errlist->Push_back(err);
	isGood = false;

	return false;
}