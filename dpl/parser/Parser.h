#pragma once
#include "ASTNode.h"
#include <vector>
#include "../lexer/LexerToken.h"

enum class ParserState {
	Code,
	VariableOperation,
	Expression,
	Arguments
};

class Parser {
public:
	ASTNode* run(std::vector<LexerToken> tokens);
private:
	template <typename T>
	T* create_new(ASTNode** current_target, LexerToken* token);
	template <typename T>
	T* create_new_dont_enter(ASTNode** current_target, LexerToken* token)
	{
		T* ret = create_new<T>(current_target, token);
		go_up(current_target);
		return ret;
	}
	void go_up(ASTNode** current_target);
};