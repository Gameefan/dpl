#pragma once
#include "../lexer/LexerToken.h"
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <cstdlib>

struct LineInfo {
	size_t line;
	size_t character;
	const char* this_line;
	const char* next_line;
};

static LineInfo get_line(LexerToken* token, const char* text)
{
	size_t index = token->offset;
	size_t num_newline = 0;
	size_t last_newline_index = 0;
	char* this_line = (char*)malloc(512);
	assert(this_line);
	size_t ptr = 0;
	for (size_t i = 0; i < index; i++)
	{
		if (text[i] == 0) {
			// FIXME: warning or something???
			return { num_newline + 1, index - last_newline_index, this_line, nullptr };
		}
		this_line[ptr++] = text[i];
		this_line[ptr] = 0;
		if (text[i] == '\n') {
			num_newline++;
			last_newline_index = i;
			ptr = 0;
			*this_line = 0;
		}
	}
	size_t i = 0;
	do
	{
		this_line[ptr++] = text[index + i];
		this_line[ptr] = 0;
	} while (text[index + (++i)] != '\n' && text[index + (i)] != 0);
	if (text[index + i] == 0) {
		return { num_newline + 1, index - last_newline_index, this_line, nullptr };
	}
	i++;
	char* next_line = (char*)malloc(512);
	assert(next_line);
	ptr = 0;
	do
	{
		next_line[ptr++] = text[index + i];
		next_line[ptr] = 0;
	} while (text[index + (++i)] != '\n' && text[index + (i)] != 0);
	return { num_newline + 1, index - last_newline_index, this_line, next_line };
}

class ErrorReporter {
public:
	static void show_errno_info()
	{
		if (errno > 0) {
			printf("\x1b[96mHint: \x1b[97merrno=%d, %s\x1b[0m\n", errno, strerror(errno));
		}
	}
	[[noreturn]] static void report_error(LexerToken* token, const char* msg)
	{
		printf("\x1b[91mError: \x1b[97m%s\x1b[0m\n", msg);
		highlight_token(token, 91);
		show_errno_info();
		abort();
	}
	[[noreturn]] static void report_internal_error(LexerToken* token, const char* msg)
	{
		printf("\x1b[91mInternal Compiler Error: \x1b[97m%s\x1b[0m\n", msg);
		highlight_token(token, 91);
		show_errno_info();
		abort();
	}
	static void report_warning(LexerToken* token, const char* msg)
	{
		printf("\x1b[93mWarning: \x1b[97m%s\x1b[0m\n", msg);
		highlight_token(token, 93);
		show_errno_info();
	}
	[[noreturn]] static void report_error(size_t offset, size_t length, const char* msg)
	{
		LexerToken* temp_token = new LexerToken();
		temp_token->offset = offset;
		temp_token->length = length;
		report_error(temp_token, msg);
	}
	[[noreturn]] static void report_internal_error(size_t offset, size_t length, const char* msg)
	{
		LexerToken* temp_token = new LexerToken();
		temp_token->offset = offset;
		temp_token->length = length;
		report_internal_error(temp_token, msg);
	}
	static void report_warning(size_t offset, size_t length, const char* msg)
	{
		LexerToken* temp_token = new LexerToken();
		temp_token->offset = offset;
		temp_token->length = length;
		report_warning(temp_token, msg);
	}
	static void highlight_token(LexerToken* token, int color = 96)
	{
		if (!s_text) {
			printf("\x1b[91mCannot highlight token because the error reporter wasn't initialized\x1b[0m\n");
			return;
		}
		if (!token) {
			printf("\x1b[91mCannot highlight token because no token was provided\x1b[0m\n");
			return;
		}
		LineInfo line_info = get_line(token, s_text);
		const char* line_data = line_info.this_line;
		const char* next_line_data = line_info.next_line;
		printf("\x1b[93m%#4llu \x1b[97m| \x1b[0m%s\n", line_info.line, line_data);
		printf("     \x1b[97m|");
		for (size_t i = 0; i < line_info.character; i++)
		{
			printf(" ");
		}
		printf("\x1b[%dm^", color);
		for (size_t i = 0; i < token->length - 1; i++)
		{
			printf("~");
		}
		printf("\x1b[0m\n");
		printf("\x1b[93m%#4llu \x1b[97m| \x1b[0m%s\n", line_info.line + 1, next_line_data);
		printf("\x1b[0m");
	}
	static void assert_or_error(bool expr, LexerToken* token, const char* msg)
	{
		if (!expr) {
			report_error(token, msg);
		}
		assert(expr);
	}
	static void assert_or_error(bool expr, size_t offset, size_t length, const char* msg)
	{
		if (!expr) {
			report_error(offset, length, msg);
		}
		assert(expr);
	}
	static void assert_or_internal_error(bool expr, LexerToken* token, const char* msg)
	{
		if (!expr) {
			report_internal_error(token, msg);
		}
		assert(expr);
	}
	static void assert_or_internal_error(bool expr, size_t offset, size_t length, const char* msg)
	{
		if (!expr) {
			report_internal_error(offset, length, msg);
		}
		assert(expr);
	}
	static void initialize(const char* text) { s_text = text; }
private:
	inline static const char* s_text = nullptr;
};
