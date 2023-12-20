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
		//Создаем свое название программы
		tokens->Pop_front();
		Token name("BEGIN", Token::Type::Id, 0, 0);
		Token semicolon(";", Token::Type::Terminal, 0, 0);
		ID progName("id" + std::to_string(ids->Length()), ID::Type::Not_defined, 0, 0);
		ids->Push_back(progName);
		tokens->Push_front(semicolon);
		tokens->Push_front(name);
	}
	else
		tokens->Pop_front();

	//<name>
	Token name = tokens->At(0);
	if (!DefineName(name, ID::Type::Prog, "global")) {
		//Ошибка
	}
	tokens->Pop_front();

	syntaxTree = new Tree(name.GetContent());

	//;
	Token semicolon = tokens->At(0);
	if (!Semicolon(semicolon)) {
		//Ошибка
	}
	tokens->Pop_front();

	//<block>		
	if (!Block(syntaxTree, "global")) {
		//Ошибка
	}

	//.
	Token point = tokens->At(0);
	if (point.GetContent() != "point") {
		//Ошибка;
	}
	tokens->Pop_front();
}

bool Syntax::DefineName(Token& name, ID::Type type, std::string area) {
	if (name.GetType() != Token::Type::Id)
		return false;

	size_t idx = std::stoi(name.GetContent().substr(2));
	ID& nameID = ids->At(idx);

	if (nameID.GetArea() != "undefined") {
		if (nameID.GetArea() == area) {
			//ОШИБКА: Повторное объявление
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

bool Syntax::Semicolon(Token semicolon) {
	return semicolon.GetContent() == ";";
}

bool Syntax::Block(Tree* tree, std::string area) {
	if (tokens->Length() == 0)		
		return false;

	Tree* definitions = tree->CreateLeft();
	Tree* operators = tree->CreateRight();

	//<constant_section>
	if (ConstantSection(definitions, area))
		definitions = definitions->CreateRight();

	//tree->Print();

	//<variables_section>
	if (VariableSection(definitions, area)) {
		//definitions->Print();
	}		

	//<procedures_section>

	//<operators_section>
	if (!OperatorsSection(operators, "OP", 1)) {
		//Ошибка
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
		//Ошибка
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
		//ОШИБКА
		isGood = false;
		return false;
	}
	tokens->Pop_front();

	//<constant_expression>	
	int val = 0; 
	if (!ConstantExpression(tree->CreateLeft(), val)) {
		//ОШИБКА
		isGood = false;
		return false;
	}

	size_t idx = std::stoi(name.GetContent().substr(2));
	ids->At(idx).SetVal(val);

	//;
	Token semicolon = tokens->At(0);
	if (!Semicolon(semicolon)) {
		//Ошибка
		isGood = false;
		return false;
	}
	tokens->Pop_front();

	return true;
}

bool Syntax::ConstantExpression(Tree* tree, int& res) {
	if (tokens->Length() == 0)
		return false;

	List<Token>* postfix = new List<Token>();
	if (!GetConstPostfix(postfix))
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

bool Syntax::GetConstPostfix(List<Token>* result) {
	List<Token> stack;

	bool exprExpected = true;
	bool isNegative = false;

	while (true) {
		if (tokens->Length() == 0)
			return false;

		Token token = tokens->At(0);

		if (token.GetContent() == "(") {
			if (!exprExpected) {
				//Ошибка
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
				//Ошибка
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
				//Ошибка
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
				//Ошибка
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
					//Ошибка
					isGood = false;
					return false;
				}
				while (tmp.GetContent() != "(") {
					tmp = stack.Pop_front();
					result->Push_back(tmp);
					if (stack.Length() == 0 && tmp.GetContent() != ")") {
						//Ошибка
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
				//Ошибка
				isGood = false;
				return false;
			}

			result->Push_back(token);
			tokens->Pop_front();
			exprExpected = false;
			continue;
		}

		if (token.GetContent() == ";") {
			while (stack.Length() != 0) {
				Token tmp = stack.Pop_front();
				if (tmp.GetContent() == "(") {
					//Ошибка
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
				//Ошибка
				isGood = false;
				return false;
			}

			return true;
		}

		//Ошибка
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
	if (var.GetContent() != "var") {
		return false;
	}
	tokens->Pop_front();

	tree->SetData("var");
	Tree* DStree = tree->CreateLeft();

	//<description_similar_var>
	size_t DSidx = 0;
	if (!DescriptionSimilarVar(DStree, area, DSidx++)) {
		//Ошибка
		isGood = false;
		return false;
	}

	//;
	Token semicolon = tokens->At(0);
	if (!Semicolon(semicolon)) {
		//Ошибка
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
			//Ошибка
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
			//Ошибка
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
		//Ошибка
		isGood = false;
		return false;
	}
	tokens->Pop_front();

	//integer
	Token integer = tokens->At(0);
	if (integer.GetContent() != "integer") {
		//Ошибка
		isGood = false;
		return false;
	}
	tokens->Pop_front();

	return true;
}

bool Syntax::OperatorsSection(Tree* tree, std::string label, size_t idx) {
	//<compound operator>
	return CompoundOperator(tree, label, idx);	
}

bool Syntax::CompoundOperator(Tree* tree, std::string label, size_t idx) {
	if (tokens->Length() == 0)
		return false;

	//begin	
	Token _begin = tokens->At(0);
	if (_begin.GetContent() != "begin") {
		//Ошибка
		isGood = false;
		return false;
	}		
	tokens->Pop_front();

	//operator
	if(Operator(tree, label, idx))
		tree = tree->CreateRight();

	//[{; <operator>}]	
	Token semicolon = tokens->At(0);	
	while (semicolon.GetContent() == ";") {
		tokens->Pop_front();		
		if (Operator(tree, label, ++idx))
			tree = tree->CreateRight();
		semicolon = tokens->At(0);
	}

	//null-operator
	NULLOP(tree, label, idx);

	//end
	Token end = tokens->At(0);
	if (end.GetContent() != "end") {
		//Ошибка
		isGood = false;
		return false;
	}
	tokens->Pop_front();

	return true;
}

bool Syntax::Operator(Tree* tree, std::string label, size_t idx) {
	//<simple operator>

	if (SimpleOperator(tree, label, idx))
		return true;

	//<complex operator>	
	//if (ComplexOperator(label + std::to_string(idx) + "_", 1)) {
	//
	//}
	return false;
}

bool Syntax::SimpleOperator(Tree* tree, std::string label, size_t idx) {
	//<assignment_operator>
	if (AssigmentOperator(tree, label, idx))
		return true;

	//<procedure_operator>
	
	//<exit_operator>
	if (ExitOperator(tree, label, idx))
		return true;

	//<null_operator>

	return false;
}

bool Syntax::AssigmentOperator(Tree* tree, std::string label, size_t idx) {
	//<variable>
	Token var = tokens->At(0);
	if (!isVar(var))
		return false;
	tokens->Pop_front();

	//:=
	Token assign = tokens->At(0);
	if (assign.GetContent() != ":=") {
		//Ошибка
		isGood = false;
		return false;
	}
	tokens->Pop_front();
	
	//<expression>
	Tree* exprTree = new Tree();
	if (!Expression(exprTree)) {
		//Ошибка
		isGood = false;
		return false;
	}

	tree->SetData(label + std::to_string(idx));
	tree = tree->CreateLeft();
	tree->SetData(":=");
	tree->CreateLeft(var.GetContent());
	tree->CreateRight();
	tree->SetRight(exprTree);

	return true;
}

bool Syntax::Expression(Tree* tree) {
	if (tokens->Length() == 0)
		return false;

	List<Token> postfix;

	if (!GetPostfix(&postfix))
		return false;

	for (size_t idx = 0; idx < postfix.Length(); ++idx)
		std::cout << postfix.At(idx).GetContent() << " ";
	std::cout << std::endl;

	return GetExpressionTree(tree, &postfix);
}

bool Syntax::GetPostfix(List<Token>* result) {
	List<Token> stack;

	bool exprExpected = true;
	bool isNegative = false;

	while (true) {
		if (tokens->Length() == 0)
			return false;

		Token token = tokens->At(0);

		if (token.GetContent() == "(") {
			if (!exprExpected) {
				//Ошибка
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
				//Ошибка
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
				//Ошибка
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
				//Ошибка
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
					//Ошибка
					isGood = false;
					return false;
				}
				while (tmp.GetContent() != "(") {
					tmp = stack.Pop_front();
					result->Push_back(tmp);
					if (stack.Length() == 0 && tmp.GetContent() != ")") {
						//Ошибка
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
				//Ошибка
				isGood = false;
				return false;
			}

			result->Push_back(token);
			tokens->Pop_front();
			exprExpected = false;
			continue;
		}

		if (token.GetContent() == ";") {
			while (stack.Length() != 0) {
				Token tmp = stack.Pop_front();
				if (tmp.GetContent() == "(") {
					//Ошибка
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
				//Ошибка
				isGood = false;
				return false;
			}

			return true;
		}

		//Ошибка
		isGood = false;
		return false;
	}
}

bool Syntax::GetExpressionTree(Tree* tree, List<Token>* postfix) {	
	Token _first = postfix->Pop_back();

	if (isVar(_first) || Constant(_first)) {
		tree->SetData(_first.GetContent());
		return true;
	}

	if (_first.GetType() != Token::Type::Operation) {
		//Ошибка
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

void Syntax::NULLOP(Tree* tree, std::string label, size_t idx) {
	tree->SetData(label + std::to_string(idx));
	tree = tree->CreateLeft();
	tree->SetData("NULL");
}