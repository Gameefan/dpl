#pragma once

#define __ENUMERATE_LEXER_TOKEN_TYPES \
	__ENUM(Invalid) \
	__ENUM(Semicolon) \
	__ENUM(VariableName) \
	__ENUM(NumericLiteral) \
	__ENUM(SelectorLiteral) \
	__ENUM(TextLiteral) \
	__ENUM(Template) \
	__ENUM(Function) \
	__ENUM(FunctionCall) \
	__ENUM(ArgsStart) \
	__ENUM(ArgsEnd) \
	__ENUM(BlockStart) \
	__ENUM(BlockEnd) \
	__ENUM(InlineStart) \
	__ENUM(InlineEnd) \
	__ENUM(Comment) \
	__ENUM(OpEquals) \
	__ENUM(OpAdd) \
	__ENUM(OpSubtract) \
	__ENUM(OpDivide) \
	__ENUM(OpMultiply) \
	__ENUM(OpModulo) \
	__ENUM(OpLess) \
	__ENUM(OpNot) \
	__ENUM(OpMore) \
	__ENUM(RawData) \
	__ENUM(YieldStatement) \
	__ENUM(IfStatement) \
	__ENUM(UnlessStatement) \
	__ENUM(EndOfFile)


enum class LexerTokenType {
#define __ENUM(x) x,
	__ENUMERATE_LEXER_TOKEN_TYPES
#undef __ENUM
};

struct LexerToken {
	LexerTokenType type;
	bool is_number_data;
	union {
		const char* text_value;
		int numeric_value;
	};
	size_t offset;
	size_t length;
	size_t line;
	LexerToken() {}
	LexerToken(LexerTokenType t, size_t token_offset, size_t token_length)
	{
		type = t;
		is_number_data = false;
		text_value = nullptr;
		offset = token_offset;
		length = token_length;
	}
	LexerToken(LexerTokenType t, const char* text, size_t token_offset, size_t token_length)
	{
		type = t;
		is_number_data = false;
		text_value = text;
		offset = token_offset;
		length = token_length;
	}
	LexerToken(LexerTokenType t, int num, size_t token_offset, size_t token_length)
	{
		type = t;
		is_number_data = true;
		numeric_value = num;
		offset = token_offset;
		length = token_length;
	}
};