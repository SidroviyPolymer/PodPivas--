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
		//Ñîçäàåì ñâîå íàçâàíèå ïðîãðàììû
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
		//Îøèáêà
	}
	tokens->Pop_front();

	syntaxTree = new Tree(name.GetContent());

	//;
	Token semicolon = tokens->At(0);
	if (!Semicolon(semicolon)) {
		//Îøèáêà
	}
	tokens->Pop_front();

	//<block>		
	if (!Block(syntaxTree, "global")) {
		//Îøèáêà
	}

	//.
	Token point = tokens->At(0);
	if (point.GetContent() != "point") {
		//Îøèáêà;
	}
	tokens->Pop_front();
}

bool Syntax::DefineName(Token& name, ID::Type type, std::string area) {
	if (name.GetType() != Token::Type::Id) {
		//Îøèáêà
		return false;
	}

	size_t idx = std::stoi(name.GetContent().substr(2));
	ID& nameID = ids->At(idx);

	if (nameID.GetArea() != "undefined") {
		if (nameID.GetArea() == area) {
			//ÎØÈÁÊÀ: Ïîâòîðíîå îáúÿâëåíèå
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
	if (OperatorsSection(operators, "OP", 1)) {
		//Îøèáêà
		return false;
	}

	return true;
}

bool Syntax::ConstantSection(Tree* tree, std::string area) {
	if (tokens->Length() == 0)
		return false;

	//const
	Token _const = tokens->At(0);
	if (_const.GetContent() != "const") {
		//NULL
		return false;
	}
	tokens->Pop_front();

	tree->SetData("const");	
	tree = tree->CreateLeft();

	//<definition_constant>
	if (!DefinitionConstant(tree, area)) {
		//Îøèáêà
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
	if (!DefineName(name, ID::Type::Const, area)) {
		//ÎØÈÁÊÀ
		return false;
	}
	tokens->Pop_front();
	tree->SetData(name.GetContent());

	//=
	Token equal = tokens->At(0);
	if (equal.GetContent() != "=") {
		//ÎØÈÁÊÀ
		return false;
	}
	tokens->Pop_front();

	//<constant_expression>	
	if (!ConstantExpression(tree->CreateLeft())) {
		//ÎØÈÁÊÀ
		return false;
	}

	//;
	Token semicolon = tokens->At(0);
	if (!Semicolon(semicolon)) {
		//Îøèáêà
		return false;
	}
	tokens->Pop_front();

	return true;
}

bool Syntax::ConstantExpression(Tree* tree) {
	if (tokens->Length() == 0)
		return false;

	Tree* exprTree = new Tree();

	//<consant_term>
	if (!ConstantTerm(exprTree->CreateLeft())) {
		//Îøèáêà
		return false;
	}

	//+
	Token lp_operator = tokens->At(0);
	if (lp_operator.GetContent() != "+" && lp_operator.GetContent() != "-") {
		Tree* tmp = new (tree) Tree(exprTree->GetLeft());
		delete exprTree;
		return true;
	}		
	tokens->Pop_front();
	exprTree->SetData(lp_operator.GetContent());

	//<constant_expression>
	if (!ConstantExpression(exprTree->CreateRight())) {
		//ÎØÈÁÊÀ
		return false;
	}

	Tree* tmp = new (tree) Tree(exprTree);
	delete exprTree;
	return true;
}

bool Syntax::ConstantTerm(Tree* tree) {
	if (tokens->Length() == 0)
		return false;

	Tree* exprTree = new Tree();

	//<constant_factor>
	if (!ConstantFactor(exprTree->CreateLeft())) {
		//Îøèáêà
		return false;
	}

	//(*, /, div, mod)
	Token hp_operator = tokens->At(0);
	if (hp_operator.GetContent() != "*" && hp_operator.GetContent() != "/" && hp_operator.GetContent() != "div" && hp_operator.GetContent() != "mod") {
		Tree* tmp = new (tree) Tree(exprTree->GetLeft());
		delete exprTree;
		return true;
	}		
	tokens->Pop_front();
	exprTree->SetData(hp_operator.GetContent());

	//<constant_term>	
	if (!ConstantTerm(exprTree->CreateRight())) {
		//Îøèáêà
		return false;
	}

	Tree* tmp = new (tree) Tree(exprTree);
	delete exprTree;
	return true;
}

bool Syntax::ConstantFactor(Tree* tree) {
	if (tokens->Length() == 0)
		return false;

	//<constant>
	Token constant = tokens->At(0);
	if (Constant(constant)) {
		if (constant.GetContent() == "-") {
			tokens->Pop_front();
			tree->SetData(constant.GetContent());
			constant = tokens->Pop_front();
			tree->CreateLeft()->SetData(constant.GetContent());
		}
		else {
			tokens->Pop_front();
			tree->SetData(constant.GetContent());
		}
		return true;
	}

	//(
	Token open = tokens->At(0);
	if (open.GetContent() != "(") {
		//Îøèáêà
		return false;
	}
	tokens->Pop_front();

	//<constant_expression>
	if (!ConstantExpression(tree)) {
		//ÎØÈÁÊÀ
		return false;
	}

	//)
	Token close = tokens->At(0);
	if (close.GetContent() != ")") {
		//Îøèáêà
		return false;
	}
	tokens->Pop_front();

	return true;
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
	//Îøèáêà
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
		//Îøèáêà
		return false;
	}

	//;
	Token semicolon = tokens->At(0);
	if (!Semicolon(semicolon)) {
		//Îøèáêà
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
			//Îøèáêà
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
	if (!DefineName(name, ID::Type::Var, area)) {
		//Îøèáêà
		return false;
	}
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
			//Îøèáêà
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
		//Îøèáêà
		return false;
	}
	tokens->Pop_front();

	//integer
	Token integer = tokens->At(0);
	if (integer.GetContent() != "integer") {
		//Îøèáêà
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
		//Îøèáêà
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
		//Îøèáêà
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
		//Îøèáêà
		return false;
	}
	tokens->Pop_front();
	
	//<expression>
	Tree* exprTree = new Tree();
	if (!Expression(exprTree)) {
		//Îøèáêà
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

	Tree* exprTree = new Tree();

	//<term>
	if (!Term(exprTree->CreateLeft())) {
		//Îøèáêà
		return false;
	}

	//+
	Token lp_operator = tokens->At(0);
	if (lp_operator.GetContent() != "+" && lp_operator.GetContent() != "-") {
		Tree* tmp = new (tree) Tree(exprTree->GetLeft());
		delete exprTree;
		return true;
	}
	tokens->Pop_front();
	exprTree->SetData(lp_operator.GetContent());

	//<expression>
	if (!Expression(exprTree->CreateRight())) {
		//ÎØÈÁÊÀ
		return false;
	}

	Tree* tmp = new (tree) Tree(exprTree);
	delete exprTree;
	return true;
}

bool Syntax::Term(Tree* tree) {
	if (tokens->Length() == 0)
		return false;

	Tree* exprTree = new Tree();

	//<factor>
	if (!Factor(exprTree->CreateLeft())) {
		//Îøèáêà
		return false;
	}

	//(*, /, div, mod)
	Token hp_operator = tokens->At(0);
	if (hp_operator.GetContent() != "*" && hp_operator.GetContent() != "/" && hp_operator.GetContent() != "div" && hp_operator.GetContent() != "mod") {
		Tree* tmp = new (tree) Tree(exprTree->GetLeft());
		delete exprTree;
		return true;
	}
	tokens->Pop_front();
	exprTree->SetData(hp_operator.GetContent());

	//<term>	
	if (!Term(exprTree->CreateRight())) {
		//Îøèáêà
		return false;
	}

	Tree* tmp = new (tree) Tree(exprTree);
	delete exprTree;
	return true;
}

bool Syntax::Factor(Tree* tree) {
	if (tokens->Length() == 0)
		return false;

	//<identificator>
	Token id = tokens->At(0);
	if (isVar(id) || Constant(id)) {
		if (id.GetContent() == "-") {
			tokens->Pop_front();
			tree->SetData(id.GetContent());
			id = tokens->Pop_front();
			tree->CreateLeft()->SetData(id.GetContent());
		}
		else {
			tokens->Pop_front();
			tree->SetData(id.GetContent());
		}
		return true;
	}

	//(
	Token open = tokens->At(0);
	if (open.GetContent() != "(") {
		//Îøèáêà
		return false;
	}
	tokens->Pop_front();

	//<expression>
	if (!Expression(tree)) {
		//ÎØÈÁÊÀ
		return false;
	}

	//)
	Token close = tokens->At(0);
	if (close.GetContent() != ")") {
		//Îøèáêà
		return false;
	}
	tokens->Pop_front();

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