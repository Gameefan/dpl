#pragma once
#include <vector>
#include "../parser/AST.h"
#include "../parser/ASTNode.h"
#include "NativeFunctions.h"

struct CodeGeneratorResult {
	const char* filename;
	std::vector<const char*> lines;
	bool contains_code = true;
};

struct EvaluationResult {
	std::vector<const char*> lines;
	const char* result_identifier;
};

struct FunctionInformation {
	const char* identifier;
	std::vector<const char*> params;
	RawStatement* template_code;
	bool is_template;
	FunctionInformation(const char* ident, std::vector<const char*> par)
	{
		identifier = ident;
		params = par;
		is_template = false;
	}
	FunctionInformation(const char* ident, RawStatement* raw)
	{
		identifier = ident;
		template_code = raw;
		is_template = true;
	}
};

class CodeGenerator {
public:
	std::vector<CodeGeneratorResult> run(Program* root_node);
	CodeGenerator(const char* package_name)
		: m_package_name(package_name)
	{}
private:
	CodeGeneratorResult prepare_global_variables(Program* program);
	EvaluationResult evaluate_math_expression(Expression* expr);
	const char* create_temporary_variable_identifier();
	int m_temporary_vars = 0;
	std::vector<FunctionInformation> prepare_functions(Program* program);
	std::vector<FunctionInformation> m_functions{};
	std::vector<CodeGeneratorResult> prepare_standard_files();
	std::vector<CodeGeneratorResult> generate_functions(Program* program);
	std::vector<CodeGeneratorResult> generate_function_declaration(FunctionDeclaration* decl);
	std::vector<CodeGeneratorResult> generate_block(ASTNode* block_parent, bool skip_first = false);
	std::vector<const char*> generate_function_call(FunctionCall* call);
	FunctionInformation* function_info_from_identifier(const char* identifier);
	std::vector<const char*> generate_assignment_expression(AssignmentExpression* expr);
	const char* generate_name_for_block(ASTNode* block_parent);
	std::vector<const char*> generate_raw_statement(RawStatement* raw);
	void prepare_notice(std::vector<CodeGeneratorResult>& files);
	const char* clear_temp_var(const char* var);

	const char* m_package_name = "dpl";
};