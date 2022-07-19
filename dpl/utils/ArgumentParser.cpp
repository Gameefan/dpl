#include "ArgumentParser.h"
#include <cstdio>
#include <cstring>
#include <cassert>

bool ArgumentParser::run(int argc, char** argv)
{
	for (size_t i = 1; i < argc; i++)
	{
		const char* arg = argv[i];
		size_t arg_len = strlen(arg);
		if (arg[0] == '-') {
			assert(arg_len == 2);
			switch (arg[1])
			{
			case 't':
				assert(!m_dump_tokens);
				m_dump_tokens = true;
				break;
			case 'a':
				assert(!m_dump_ast);
				m_dump_ast = true;
				break;
			case 'T':
				assert(!m_dump_ast_tokens);
				m_dump_ast_tokens = true;
				break;
			case 'o':
				assert(!m_optimize_ast);
				m_optimize_ast = true;
				break;
			case 'O':
				assert(!m_dump_optimized_ast);
				m_dump_optimized_ast = true;
				break;
			case 'h':
				show_help(argv[0]);
				return false;
			default:
				printf("Error: Found unknown argument: %s\n", arg);
				break;
			}
		}
		else {
			printf("Found file: %s\n", arg);
			m_file = arg;
			return true;
		}
	}
	printf("Error: No input file was provided\n");
	show_help(argv[0]);
	return false;
}

void ArgumentParser::show_help(const char* progname)
{
	printf("Usage: %s [options]... <file>\n", progname);
	printf("Transpiles .dpl files to .mcfunction files and puts them into a zip archive for easy addition to a Minecraft world.\n");
	printf("\n");
	printf("\t-t\tDump the lexer output\n");
	printf("\t-a\tDump the parser output\n");
	printf("\t-T\tDump the AST tokens when dumping parser output\n");
	printf("\t-o\tOptimize the parsed AST\n");
	printf("\t-O\tDump the optimized AST\n");
	printf("\t-h\tShow this help message\n");
}
