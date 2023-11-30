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

void Syntax::Program() {
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
	Tree* definitions = tree->CreateLeft();

	//<constant_section>
	if (ConstantSection(definitions, area))
		definitions = definitions->CreateRight();
}

bool Syntax::ConstantSection(Tree* tree, std::string area) {
	//const
	Token _const = tokens->At(0);
	if (_const.GetContent() != "const") {
		//NULL
		return false;
	}
	tokens->Pop_front();

	tree->GetData() = "const";	
	tree = tree->CreateLeft();

	if (!DefinitionConstant(tree, area)) {
		//Îøèáêà
		return false;
	}	
	
	tree = tree->CreateRight();

	while (DefinitionConstant(tree, area)) {

	}
}

bool Syntax::DefinitionConstant(Tree* tree, std::string area) {
	//<name>
	Token name = tokens->At(0);
	if (!DefineName(name, ID::Type::Const, area)) {
		//ÎØÈÁÊÀ
		return false;
	}
	tokens->Pop_front();
	tree->CreateLeft();

	//=
	Token equal = tokens->At(0);
	if (equal.GetContent() != "=") {
		//ÎØÈÁÊÀ
		return false;
	}
	tokens->Pop_front();

	//<constant>
	Token constant = tokens->At(0);
	if (!Constant(constant)) {
		//ÎØÈÁÊÀ
		return false;
	}
	tokens->Pop_front();

	//;
	Token semicolon = tokens->At(0);
	if (!Semicolon(semicolon)) {
		//Îøèáêà
		return false;
	}
	tokens->Pop_front();

	return true;
}

bool Syntax::Constant(Token constant) {

}
