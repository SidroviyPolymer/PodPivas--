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
		//—ÓÁ‰‡ÂÏ Ò‚ÓÂ Ì‡Á‚‡ÌËÂ ÔÓ„‡ÏÏ˚
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
	if (!Block(syntaxTree, "global")) {
		//Œ¯Ë·Í‡
	}
}

bool Syntax::DefineName(Token& name, ID::Type type, std::string area) {
	if (name.GetType() != Token::Type::Id) {
		//Œ¯Ë·Í‡
		return false;
	}

	size_t idx = std::stoi(name.GetContent().substr(2));
	ID& nameID = ids->At(idx);

	if (nameID.GetArea() != "undefined") {
		if (nameID.GetArea() == area) {
			//Œÿ»¡ ¿: œÓ‚ÚÓÌÓÂ Ó·˙ˇ‚ÎÂÌËÂ
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

	//<constant_section>
	if (ConstantSection(definitions, area))
		definitions = definitions->CreateRight();

	//<variables_section>
	if (VariableSection(definitions, area))
		definitions = definitions->CreateRight();

	//<procedures_section>

	//<operators_section>

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
		//Œ¯Ë·Í‡
		return false;
	}	
	
	tree = tree->CreateRight();

	//[{<definition_constant>}]
	while (DefinitionConstant(tree, area)) {
		tree = tree->CreateRight();
	}

	delete tree;
}

bool Syntax::DefinitionConstant(Tree* tree, std::string area) {
	if (tokens->Length() == 0)
		return false;

	//<name>
	Token name = tokens->At(0);
	if (!DefineName(name, ID::Type::Const, area)) {
		//Œÿ»¡ ¿
		return false;
	}
	tokens->Pop_front();
	tree->SetData(name.GetContent());

	//=
	Token equal = tokens->At(0);
	if (equal.GetContent() != "=") {
		//Œÿ»¡ ¿
		return false;
	}
	tokens->Pop_front();

	//<constant_expression>	
	if (!ConstantExpression(tree->CreateLeft())) {
		//Œÿ»¡ ¿
		return false;
	}

	//;
	Token semicolon = tokens->At(0);
	if (!Semicolon(semicolon)) {
		//Œ¯Ë·Í‡
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
		//Œ¯Ë·Í‡
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
		//Œÿ»¡ ¿
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
		//Œ¯Ë·Í‡
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
		//Œ¯Ë·Í‡
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
		//Œ¯Ë·Í‡
		return false;
	}
	tokens->Pop_front();

	//<constant_expression>
	if (!ConstantExpression(tree)) {
		//Œÿ»¡ ¿
		return false;
	}

	//)
	Token close = tokens->At(0);
	if (close.GetContent() != ")") {
		//Œ¯Ë·Í‡
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
	else if (constant.GetType() == Token::Type::Id) {
		size_t idx = std::stoi(constant.GetContent().substr(2));
		ID& id = ids->At(idx);
		return id.GetType() == ID::Type::Const;
	}
	//<sign><constant_name>
	else if (constant.GetContent() == "-" || constant.GetContent() == "+") {
		return Constant(tokens->At(1));
	}
	else {
		//Œ¯Ë·Í‡
		return false;
	}
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
	tree = tree->CreateLeft();

	//<description_similar_var>
	if (!DescriptionSimilarVar(tree, area)) {
		//Œ¯Ë·Í‡
		return false;
	}

	//[<description_similar_var>]
}

bool Syntax::DescriptionSimilarVar(Tree* tree, std::string area) {

}