#include "Lexer.h"
#include <cassert>
#include "../utils/ErrorReporter.h"

std::vector<LexerToken> Lexer::run(const char* data)
{
	std::vector<LexerToken> tokens{};
	LexerState state = LexerState::GlobalCode;
	char* ptr = (char*)data;
	char temp[512]{ 0 };
	LexerState general_return_state = LexerState::GlobalCode;
	LexerState literal_return_state = LexerState::GlobalCode;
	LexerState variable_return_state = LexerState::GlobalCode;
	size_t block_depth = 0;
	do
	{
		size_t token_offset = ptr - data;
		char c = *ptr;
		switch (state)
		{
		case LexerState::GlobalCode:
			if (isspace(c)) continue;
			if (c == '#') {
				size_t len = 0;
				while (ptr[len++] != '\n') {};
				len -= 1;
				ptr += len;
				break;
			}
			if (c == '$') {
				state = LexerState::VariableName;
				variable_return_state = LexerState::GlobalCode;
				continue;
			}
			if (c == 'f') {
				char* keyword = (char*)malloc(9); // function + null
				ErrorReporter::assert_or_internal_error(keyword, token_offset, 1, "Failed to malloc");
				assert(keyword);
				strncpy(keyword, ptr, 8);
				keyword[8] = 0;
				if (strcmp(keyword, "function") == 0) {
					state = LexerState::FunctionName;
					ptr += 7;
					free(keyword);
					break;
				}
				free(keyword);
			}
			if (c == 't') {
				char* keyword = (char*)malloc(9); // template + null
				ErrorReporter::assert_or_internal_error(keyword, token_offset, 1, "Failed to malloc");
				assert(keyword);
				strncpy(keyword, ptr, 8);
				keyword[8] = 0;
				if (strcmp(keyword, "template") == 0) {
					state = LexerState::TemplateName;
					ptr += 7;
					free(keyword);
					break;
				}
				free(keyword);
			}
			if (c == 0) {
				tokens.push_back({ LexerTokenType::EndOfFile, token_offset, 1 });
				break;
			}
			ErrorReporter::report_error(token_offset, 1, "Unexpected token in global scope");
			break;
		case LexerState::VariableName:
		{
			size_t len = 0;
			while (is_valid_identifier_character(ptr[len++])) {};
			len -= 1;
			if (len == 0) {
				ErrorReporter::report_error(token_offset, 1, "Variable name must not be empty");
			}
			char* vname = (char*)malloc(len + 1);
			ErrorReporter::assert_or_internal_error(vname, token_offset, len, "Failed to malloc");
			assert(vname);
			strncpy(vname, ptr, len);
			vname[len] = 0;
			tokens.push_back({ LexerTokenType::VariableName, vname, token_offset, len });
			ptr += len - 1;
			if (variable_return_state == LexerState::GlobalCode) {
				state = LexerState::AfterGlobalVariable;
			}
			else if (variable_return_state == LexerState::LocalCode) {
				state = LexerState::AfterLocalVariable;
			}
			else if (variable_return_state == LexerState::DefinitionArguments) {
				state = LexerState::DefinitionArguments;
			}
			else if (variable_return_state == LexerState::CallArguments) {
				state = LexerState::CallArguments;
			}
			else if (variable_return_state == LexerState::InlineStatement) {
				state = LexerState::InlineStatement;
			}
			else if (variable_return_state == LexerState::AfterLocalVariable) {
				state = LexerState::AfterLocalVariable;
			}
			else {
				ErrorReporter::report_error(token_offset, len, "Unexpected value of variable_return_state");
			}
			break;
		}
		case LexerState::AfterGlobalVariable:
			if (isspace(c)) continue;
			if (c == '=') {
				tokens.push_back({ LexerTokenType::OpEquals, token_offset, 1 });
				state = LexerState::NumericLiteral;
				literal_return_state = LexerState::Semicolon;
				break;
			}
			ErrorReporter::report_error(token_offset, 1, "Unexpected token after variable name, GlobalCode may only set variables to constant numeric values");
			break;
		case LexerState::AfterLocalVariable:
			if (isspace(c)) continue;
			if (c == '=') {
				tokens.push_back({ LexerTokenType::OpEquals, token_offset, 1 });
				break;
			}
			if (c == '+') {
				tokens.push_back({ LexerTokenType::OpAdd, token_offset, 1 });
				break;
			}
			if (c == '-') {
				tokens.push_back({ LexerTokenType::OpSubtract, token_offset, 1 });
				break;
			}
			if (c == '*') {
				tokens.push_back({ LexerTokenType::OpMultiply, token_offset, 1 });
				break;
			}
			if (c == '/') {
				tokens.push_back({ LexerTokenType::OpDivide, token_offset, 1 });
				break;
			}
			if (c == '%') {
				tokens.push_back({ LexerTokenType::OpModulo, token_offset, 1 });
				break;
			}
			if (c == '(') {
				tokens.push_back({ LexerTokenType::InlineStart, token_offset, 1 });
				break;
			}
			if (c == ')') {
				tokens.push_back({ LexerTokenType::InlineEnd, token_offset, 1 });
				break;
			}
			if (isdigit(c)) {
				state = LexerState::NumericLiteral;
				literal_return_state = LexerState::AfterLocalVariable;
				ptr--;
				break;
			}
			if (c == '$')
			{
				state = LexerState::VariableName;
				variable_return_state = LexerState::AfterLocalVariable;
				break;
			}
			if (c == ';')
			{
				ptr--;
				state = LexerState::Semicolon;
				general_return_state = LexerState::LocalCode;
				break;
			}
			if (isalpha(c))
			{
				ptr--;
				state = LexerState::FunctionName;
				break;
			}
			ErrorReporter::report_error(token_offset, 1, "Unexpected token after variable name");
			break;
		case LexerState::TextLiteral:
		{
			if (isspace(c)) continue;
			size_t len = 0;
			while (ptr[len++] != '"') {};
			len -= 1;
			char* text = (char*)malloc(len + 1);
			ErrorReporter::assert_or_internal_error(text, token_offset, len, "Failed to malloc");
			assert(text);
			strncpy(text, ptr, len);
			text[len] = 0;
			tokens.push_back({ LexerTokenType::TextLiteral, text, token_offset, len });
			ptr += len; // a small difference
			state = literal_return_state;
			break;
		}
		case LexerState::NumericLiteral:
		{
			if (isspace(c)) continue;
			if (isalpha(c)) {
				ErrorReporter::report_error(token_offset, 1, "Found a text literal while a numeric literal was expected");
			}
			size_t len = 0;
			while (isdigit(ptr[len++])) {};
			len -= 1;
			if (len == 0) {
				ErrorReporter::report_error(token_offset, 1, "Couldn't find numeric value");
			}
			char* nval = (char*)malloc(len + 1);
			ErrorReporter::assert_or_internal_error(nval, token_offset, len, "Failed to malloc");
			assert(nval);
			strncpy(nval, ptr, len);
			nval[len] = 0;
			int value = atoi(nval);
			free(nval);
			tokens.push_back({ LexerTokenType::NumericLiteral, value, token_offset, len });
			ptr += len - 1;
			state = literal_return_state;
			break;
		}
		case LexerState::SelectorLiteral:
		{
			size_t len = 0;
			bool opened_bracket = false;
			bool closed_bracket = false;
			while (true)
			{
				if (*ptr == '[')
				{
					opened_bracket = true;
				}
				if (*ptr == ']' && opened_bracket)
				{
					closed_bracket = true;
					break;
				}
				if (isspace(ptr[len])) break;
				if (ptr[len] == ')') break;
				if (!opened_bracket || closed_bracket) {
					if (c == ',') {
						break;
					}
				}
				len++;
			}
			if (len == 0) {
				ErrorReporter::report_error(token_offset, 1, "Couldn't find a selector");
			}
			char* text = (char*)malloc(len + 1);
			ErrorReporter::assert_or_internal_error(text, token_offset, len, "Failed to malloc");
			assert(text);
			strncpy(text, ptr, len);
			text[len] = 0;
			tokens.push_back({ LexerTokenType::SelectorLiteral, text, token_offset - 1, len + 1 });
			ptr += len - 1;
			state = literal_return_state;
			break;
		}
		case LexerState::Semicolon:
			if (isspace(c)) continue;
			if (c == ';') {
				tokens.push_back({ LexerTokenType::Semicolon, token_offset, 1 });
				state = general_return_state;
				break;
			}
			ErrorReporter::report_error(token_offset, 1, "Missing semicolon");
			break;
		case LexerState::FunctionName:
		{
			if (isspace(c)) continue;
			size_t len = 0;
			while (is_valid_identifier_character(ptr[len++])) {};
			len -= 1;
			if (len == 0) {
				ErrorReporter::report_error(token_offset, 1, "Function name must not be empty");
			}
			char* fname = (char*)malloc(len + 1);
			ErrorReporter::assert_or_internal_error(fname, token_offset, len, "Failed to malloc");
			assert(fname);
			strncpy(fname, ptr, len);
			fname[len] = 0;

			ptr += len;
			while (isspace(*ptr)) { ptr++; };
			if (general_return_state == LexerState::GlobalCode) {
				state = LexerState::DefinitionArguments;
				tokens.push_back({ LexerTokenType::Function, fname, token_offset, len });
				ptr -= 1;
			}
			else if (general_return_state == LexerState::LocalCode) {
				state = LexerState::CallArguments;
				tokens.push_back({ LexerTokenType::FunctionCall, fname, token_offset, len });
				ptr -= 1;
			}
			else {
				ErrorReporter::report_error(token_offset, 1, "Invalid general_return_state");
			}
			break;
		}
		case LexerState::TemplateName:
		{
			if (isspace(c)) continue;
			size_t len = 0;
			while (is_valid_identifier_character(ptr[len++])) {};
			len -= 1;
			if (len == 0) {
				ErrorReporter::report_error(token_offset, 1, "Template name must not be empty");
			}
			char* tname = (char*)malloc(len + 1);
			ErrorReporter::assert_or_internal_error(tname, token_offset, len, "Failed to malloc");
			assert(tname);
			strncpy(tname, ptr, len);
			tname[len] = 0;

			ptr += len;
			while (isspace(*ptr)) { ptr++; };
			state = LexerState::RawData;
			general_return_state = LexerState::GlobalCode;
			tokens.push_back({ LexerTokenType::Template, tname, token_offset, len });
			break;
		}
		case LexerState::DefinitionArguments:
			if (c == ',') continue;
			if (isspace(c)) continue;
			if (c == '(') {
				tokens.push_back({ LexerTokenType::ArgsStart, token_offset, 1 });
				break;
			}
			if (c == ')') {
				tokens.push_back({ LexerTokenType::ArgsEnd, token_offset, 1 });
				state = LexerState::BlockStart;
				general_return_state = LexerState::LocalCode;
				literal_return_state = LexerState::LocalCode;
				break;
			}
			variable_return_state = LexerState::DefinitionArguments;
			state = LexerState::VariableName;
			ptr--;
			break;
		case LexerState::CallArguments:
			if (c == ',') continue;
			if (isspace(c)) continue;
			if (c == '(') {
				tokens.push_back({ LexerTokenType::ArgsStart, token_offset, 1 });
				break;
			}
			if (c == ')') {
				tokens.push_back({ LexerTokenType::ArgsEnd, token_offset, 1 });
				state = LexerState::Semicolon;
				general_return_state = LexerState::LocalCode;
				literal_return_state = LexerState::LocalCode;
				break;
			}
			if (c == '$') {
				variable_return_state = LexerState::CallArguments;
				state = LexerState::VariableName;
				break;
			}
			if (c == '\"') {
				literal_return_state = LexerState::CallArguments;
				state = LexerState::TextLiteral;
				break;
			}
			if (isdigit(c)) {
				literal_return_state = LexerState::CallArguments;
				state = LexerState::NumericLiteral;
				ptr--;
				break;
			}
			if (c == '@') {
				literal_return_state = LexerState::CallArguments;
				state = LexerState::SelectorLiteral;
				break;
			}
			ErrorReporter::report_error(token_offset, 1, "Encountered unexpected token while parsing call arguments");
			break;
		case LexerState::InlineStatement:
			if (isspace(c)) continue;
			if (c == '$') {
				variable_return_state = LexerState::InlineStatement;
				state = LexerState::VariableName;
				break;
			}
			if (isdigit(c)) {
				literal_return_state = LexerState::InlineStatement;
				state = LexerState::NumericLiteral;
				ptr--;
				break;
			}
			if (c == '>') {
				tokens.push_back({ LexerTokenType::OpMore, token_offset, 1 });
				break;
			}
			if (c == '<') {
				tokens.push_back({ LexerTokenType::OpLess, token_offset, 1 });
				break;
			}
			if (c == '=') {
				tokens.push_back({ LexerTokenType::OpEquals, token_offset, 1 });
				break;
			}
			if (c == '+') {
				tokens.push_back({ LexerTokenType::OpAdd, token_offset, 1 });
				break;
			}
			if (c == '-') {
				tokens.push_back({ LexerTokenType::OpSubtract, token_offset, 1 });
				break;
			}
			if (c == '*') {
				tokens.push_back({ LexerTokenType::OpMultiply, token_offset, 1 });
				break;
			}
			if (c == '/') {
				tokens.push_back({ LexerTokenType::OpDivide, token_offset, 1 });
				break;
			}
			if (c == '%') {
				tokens.push_back({ LexerTokenType::OpModulo, token_offset, 1 });
				break;
			}
			if (c == '!') {
				tokens.push_back({ LexerTokenType::OpNot, token_offset, 1 });
				break;
			}
			if (c == '(') {
				ErrorReporter::report_error(token_offset, 1, "Nested inline statements aren't supported yet");
			}
			if (c == ')') {
				tokens.push_back({ LexerTokenType::InlineEnd, token_offset, 1 });
				state = general_return_state;
				break;
			}
			if (c == ';') {
				tokens.push_back({ LexerTokenType::InlineEnd, token_offset, 1 });
				state = LexerState::Semicolon;
				ptr--;
				break;
			}
			ErrorReporter::report_error(token_offset, 1, "Unexpected token inside InlineStatement");
			break;
		case LexerState::BlockStart:
			if (isspace(c)) continue;
			if (c == '{')
			{
				tokens.push_back({ LexerTokenType::BlockStart, token_offset, 1 });
				state = LexerState::LocalCode;
				block_depth++;
				break;
			}
			ErrorReporter::report_error(token_offset, 1, "Unexpected token while looking for block start");
			break;
		case LexerState::LocalCode:
			if (isspace(c)) continue;
			if (c == '#') {
				size_t len = 0;
				while (ptr[len++]!='\n') {};
				len -= 1;
				ptr += len;
				break;
			}
			if (c == '$') {
				variable_return_state = LexerState::LocalCode;
				state = LexerState::VariableName;
				break;
			}
			if (c == 'r') {
				char* keyword = (char*)malloc(4); // raw + null
				ErrorReporter::assert_or_internal_error(keyword, token_offset, 1, "Failed to malloc");
				assert(keyword);
				strncpy(keyword, ptr, 3);
				keyword[3] = 0;
				if (strcmp(keyword, "raw") == 0) {
					ptr += 3;
					if (isspace(*ptr)) while (isspace(*(++ptr))) {};
					state = LexerState::RawData;
					general_return_state = LexerState::LocalCode;
					// Don't decrement the pointer, we want to skip the '{'
					free(keyword);
					break;
				}
				free(keyword);
			}
			if(c=='y')
			{
				char* keyword = (char*)malloc(6); // yield + null
				ErrorReporter::assert_or_internal_error(keyword, token_offset, 1, "Failed to malloc");
				assert(keyword);
				strncpy(keyword, ptr, 5);
				keyword[5] = 0;
				if (strcmp(keyword, "yield") == 0) {
					ptr += 5;
					if (isspace(*ptr)) while (isspace(*(++ptr))) {};
					tokens.push_back({ LexerTokenType::YieldStatement, token_offset, 5 });
					tokens.push_back({ LexerTokenType::InlineStart, token_offset + 5, 1 });
					state = LexerState::InlineStatement;
					general_return_state = LexerState::LocalCode;
					ptr--;
					free(keyword);
					break;
				}
				free(keyword);
			}
			if (c == 'i') {
				char* keyword = (char*)malloc(3); // if + null
				ErrorReporter::assert_or_internal_error(keyword, token_offset, 1, "Failed to malloc");
				assert(keyword);
				strncpy(keyword, ptr, 2);
				keyword[2] = 0;
				if (strcmp(keyword, "if") == 0) {
					ptr += 2;
					if (isspace(*ptr)) while (isspace(*(++ptr))) {};
					tokens.push_back({ LexerTokenType::IfStatement, token_offset, 2 });
					tokens.push_back({ LexerTokenType::InlineStart, token_offset + 2, 1 });
					state = LexerState::InlineStatement;
					general_return_state = LexerState::LocalCode;
					free(keyword);
					break;
				}
				free(keyword);
			}
			if (c == 'u') {
				char* keyword = (char*)malloc(7); // unless + null
				ErrorReporter::assert_or_internal_error(keyword, token_offset, 1, "Failed to malloc");
				assert(keyword);
				strncpy(keyword, ptr, 6);
				keyword[6] = 0;
				if (strcmp(keyword, "unless") == 0) {
					ptr += 6;
					if (isspace(*ptr)) while (isspace(*(++ptr))) {};
					tokens.push_back({ LexerTokenType::UnlessStatement, token_offset, 6 });
					tokens.push_back({ LexerTokenType::InlineStart, token_offset + 6, 1 });
					state = LexerState::InlineStatement;
					general_return_state = LexerState::LocalCode;
					free(keyword);
					break;
				}
				free(keyword);
			}
			if (c == '{') {
				block_depth++;
				tokens.push_back({ LexerTokenType::BlockStart, token_offset, 1 });
				break;
			}
			if (c == '}')
			{
				block_depth--;
				if (block_depth == 0) {
					// The function ended
					tokens.push_back({ LexerTokenType::BlockEnd, token_offset, 1 });
					state = LexerState::GlobalCode;
					general_return_state = LexerState::GlobalCode;
					literal_return_state = LexerState::GlobalCode;
					break;
				}
				else {
					tokens.push_back({ LexerTokenType::BlockEnd, token_offset, 1 });
					break;
				}
			}
			// Random semicolons are fine
			if (c == ';') continue;
			general_return_state = LexerState::LocalCode;
			state = LexerState::FunctionName;
			ptr--;
			break;
		case LexerState::RawData:
		{
			if (isspace(*ptr)) while (isspace(*(++ptr))) {};
			size_t paren_count = 1;
			char tmp[1024];
			char* tmp_ptr = tmp;
			while (true)
			{
				if (*ptr == '{') paren_count++;
				if (*ptr == '}') paren_count--;
				if (paren_count == 0) break;
				*tmp_ptr++ = *ptr++;
				*tmp_ptr = 0;
			}
			size_t text_len = (size_t)(tmp_ptr - tmp);
			char* m = (char*)malloc(1024);
			ErrorReporter::assert_or_internal_error(m, token_offset, text_len, "Failed to malloc");
			assert(m);
			strcpy(m, tmp);
			ptr++;
			tokens.push_back({ LexerTokenType::RawData, m, token_offset, text_len });
			if (isspace(*ptr)) while (isspace(*(++ptr))) {};
			if (isspace(*ptr)) while (isspace(*(++ptr))) {};
			if (*ptr == '%')
			{
				state = LexerState::CallArguments;
				break;
			}
			ptr -= 2;
			state = general_return_state;
			break;
		}
		default:
			ErrorReporter::report_error(token_offset, 1, "Unknown lexer state");
			break;
		}
	} while (*ptr++ != 0);
	return tokens;
}

void Lexer::dump_tokens(std::vector<LexerToken> tokens)
{
	for (auto tok : tokens)
	{
		if (tok.is_number_data)
			printf("- (%#5llu+%#3llu) %s (%d)\n", tok.offset, tok.length, Lexer::token_type_to_string(tok.type), tok.numeric_value);
		else if (tok.text_value)
			printf("- (%#5llu+%#3llu) %s (%s)\n", tok.offset, tok.length, Lexer::token_type_to_string(tok.type), tok.text_value);
		else
			printf("- (%#5llu+%#3llu) %s\n", tok.offset, tok.length, Lexer::token_type_to_string(tok.type));
	}
}