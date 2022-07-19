cmake_minimum_required (VERSION 3.8)

configure_file(test.dpl test.dpl COPYONLY)
configure_file(inc.dpl inc.dpl COPYONLY)

add_executable (dpl "dpl.cpp"
					"utils/FileLoader.cpp"
					"utils/FileLoader.h"
					"preprocessor/Preprocessor.cpp"
					"preprocessor/Preprocessor.h"
					"lexer/Lexer.cpp"
					"lexer/Lexer.h"
					"lexer/LexerToken.h"
					"parser/Parser.cpp"
					"parser/Parser.h"
					"parser/AST.h"
					"parser/ASTNode.cpp"
					"parser/ASTNode.h"
					"codegen/CodeGenerator.cpp"
					"codegen/CodeGenerator.h"
					"codegen/NativeFunctions.h"
					"codegen/Validator.h"
					"exporter/Exporter.cpp"
					"exporter/Exporter.h"
					"utils/ArgumentParser.cpp"
					"utils/ArgumentParser.h"
					"utils/ErrorReporter.h" "optimizer/Optimizer.cpp" "optimizer/Optimizer.h")