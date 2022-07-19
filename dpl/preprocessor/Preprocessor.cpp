#include "Preprocessor.h"
#include <malloc.h>
#include <cassert>
#include <cstdio>
#include <cstring>
#include "../utils/FileLoader.h"

const char* Preprocessor::run(const char* data)
{
	size_t current_alloc_size = 64;
	char* ret = (char*)malloc(current_alloc_size);
	*ret = 0;
	assert(ret);
	size_t ptr = 0;
	size_t write_ptr = 0;
	bool parsing_pp = false;
	bool parsing_var = false;
	size_t if_depth = 0;
	bool is_skipping = false;
	size_t skipping_from = 0;
	do
	{
		char c = data[ptr];
		if (parsing_pp) {
			char tmp[512];
			size_t tmp_ptr = 0;
			do
			{
				tmp[tmp_ptr++] = data[ptr];
				tmp[tmp_ptr] = 0;
				ptr++;
			} while (data[ptr - 1] != '\n' && data[ptr - 1] != 0 && !(parsing_var && (data[ptr - 1] == ' ' || data[ptr - 1] == ';')));
			if (tmp[tmp_ptr - 1] == ';') {
				tmp[tmp_ptr - 1] = 0;
				ptr--;
			}
			if (tmp[tmp_ptr - 1] == '\n') tmp[tmp_ptr - 1] = 0;
			if (tmp[tmp_ptr - 2] == '\r') tmp[tmp_ptr - 2] = 0;
			ptr--;
			PreprocessorParsingResult ppr = parse_preprocessor_instruction(tmp);
			if (ppr.is_if) {
				if_depth++;
			}
			if (ppr.is_endif) {
				if (if_depth == 0) {
					assert(false);
				}
				if_depth--;
			}
			if (ppr.skip_to_endif)
			{
				// Skip to my endif on the stack
				skipping_from = if_depth;
				is_skipping = true;
			}
			if (if_depth < skipping_from)
			{
				is_skipping = false;
			}
			if (ppr.text_to_place&&!is_skipping)
			{
				size_t text_len = strlen(ppr.text_to_place);
				if (write_ptr + 10 + text_len >= current_alloc_size)
				{
					current_alloc_size += 64;
					current_alloc_size += text_len;
					ret = (char*)realloc(ret, current_alloc_size);
					assert(ret);
				}
				strcat(ret, ppr.text_to_place);
				write_ptr += text_len;
			}
			parsing_pp = false;
		}
		else
		{
			if (write_ptr + 10 >= current_alloc_size)
			{
				current_alloc_size += 64;
				ret = (char*)realloc(ret, current_alloc_size);
				assert(ret);
			}
			if (c == '%')
			{
				if ((ptr == 0 || data[ptr - 1] == '\n') || data[ptr + 1] == '%') {
					parsing_pp = true;
					parsing_var = data[ptr + 1] == '%';
					continue;
				}
			}
			if (!is_skipping) {
				ret[write_ptr++] = c;
				ret[write_ptr] = 0;
			}
		}
	} while (data[++ptr] != 0);

	return ret;
}

Preprocessor::PreprocessorParsingResult Preprocessor::parse_preprocessor_instruction(const char* text)
{
	PreprocessorParsingResult ret{};
	printf("Preprocessor: Parsing instruction: \"%s\"\n", text);
	if (text[0] == '%')
	{
		ret.text_to_place = "__TODO_PP_DEF__";
		return ret;
	}
	// define ABC 123
	char* tmp = strdup(text);
	char* argv[3];
	int argc = 0;
	char* context = nullptr;
	char* token = strtok_s(tmp, " ", &context);
	while (token)
	{
		//printf(" - %s\n", token);
		argv[argc++] = token;
		token = strtok_s(nullptr, " ", &context);
	}
	assert(argc > 0);
	if (strcmp(argv[0], "define")==0)
	{
		assert(argc >= 2);
		// TODO: Implement this
		return {};
	}
	if (strcmp(argv[0], "ifdef") == 0)
	{
		assert(argc == 2);
		ret.is_if = true;
		// TODO: For now everything is undefined
		ret.skip_to_endif = true;
		return ret;
	}
	if (strcmp(argv[0], "endif") == 0)
	{
		assert(argc == 1);
		ret.is_endif = true;
		return ret;
	}
	if (strcmp(argv[0], "include") == 0)
	{
		assert(argc == 2);
		ret.text_to_place = FileLoader::load_file(argv[1]);
		return ret;
	}
	return {};
}
