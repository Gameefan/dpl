#include <cstdio>
#include <cassert>
#include "preprocessor/Preprocessor.h"
#include "lexer/Lexer.h"
#include "utils/FileLoader.h"
#include "parser/Parser.h"
#include "parser/ASTNode.h"
#include "codegen/CodeGenerator.h"
#include "codegen/Validator.h"
#include "exporter/Exporter.h"
#include "utils/ErrorReporter.h"
#include "utils/ArgumentParser.h"
#include "optimizer/Optimizer.h"

int main(int argc, char** argv)
{
	ArgumentParser* arg_parser = new ArgumentParser();
	if (!arg_parser->run(argc, argv))
	{
		return 1;
	}

	Preprocessor* pp = new Preprocessor();
	const char* input_data = FileLoader::load_file(arg_parser->file());
	const char* pp_out = pp->run(input_data);

	ErrorReporter::initialize(pp_out);

	Lexer* lex = new Lexer();
	std::vector<LexerToken> lex_out = lex->run(pp_out);
	if (arg_parser->dump_tokens()) {
		Lexer::dump_tokens(lex_out);
	}

	Parser* parser = new Parser();
	ASTNode* root_node = parser->run(lex_out);
	assert(root_node);
	if (arg_parser->dump_ast()) {
		root_node->dump(0, arg_parser->dump_ast_tokens());
	}

	Validator* valid = new Validator();
	valid->run(root_node->as<Program>());

	if (arg_parser->optimize_ast()) {
		Optimizer* opt = new Optimizer();
		root_node = opt->run(root_node);
		ErrorReporter::assert_or_internal_error(root_node->is<Program>(), root_node->token(), "Node returned from the Optimizer isn't a Program");

		if (arg_parser->dump_optimized_ast())
		{
			root_node->dump();
		}
	}

	CodeGenerator* gen = new CodeGenerator();
	std::vector<CodeGeneratorResult> code_gen_results = gen->run(root_node->as<Program>());

	Exporter* exporter = new Exporter();
	exporter->run(code_gen_results);

	return 0;
}
