#pragma once
#include <vector>
#include <cctype>
#include "LexerToken.h"
#include <cassert>
#include <cstdlib>
#include <cstdio>

enum class LexerState
{
	GlobalCode,
	VariableName,
	AfterGlobalVariable,
	AfterLocalVariable,
	TextLiteral,
	NumericLiteral,
	SelectorLiteral,
	Semicolon,
	FunctionName,
	TemplateName,
	DefinitionArguments,
	CallArguments,
	InlineStatement,
	BlockStart,
	LocalCode,
	RawData
};

class Lexer {
public:
	std::vector<LexerToken> run(const char* data);
	static const char* token_type_to_string(LexerTokenType type)
	{
		switch (type)
		{
#define __ENUM(x) \
		case LexerTokenType::x: \
			return #x;
			__ENUMERATE_LEXER_TOKEN_TYPES
#undef __ENUM
		default:
			printf("Cannot convert unknown token type to string");
			abort();
			break;
		}
	}
	static void dump_tokens(std::vector<LexerToken> tokens);
private:
	bool is_valid_identifier_character(char c) { return isalnum(c) || c == '_' || c == '.'; }
};
