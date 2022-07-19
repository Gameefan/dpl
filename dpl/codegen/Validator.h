#pragma once
#include "../parser/ASTNode.h"
#include "../parser/AST.h"
#include "../utils/ErrorReporter.h"

class Validator {
public:
	void run(Program* program)
	{
		validate(program);
	}
private:
	void validate(ASTNode* node)
	{
		// printf("Validating %s\n", node->node_name());
		for (size_t i = 0; i < node->children().size(); i++)
		{
			validate(node->children()[i]);
		}

		if (node->is<AddExpression>()) return validate_standard_expression(node->as<Expression>());
		if (node->is<SubtractExpression>()) return validate_standard_expression(node->as<Expression>());
		if (node->is<MultiplyExpression>()) return validate_standard_expression(node->as<Expression>());
		if (node->is<DivideExpression>()) return validate_standard_expression(node->as<Expression>());
		if (node->is<ModuloExpression>()) return validate_standard_expression(node->as<Expression>());
		if (node->is<MoreExpression>()) return validate_standard_expression(node->as<Expression>());
		if (node->is<MoreEqualsExpression>()) return validate_standard_expression(node->as<Expression>());
		if (node->is<LessExpression>()) return validate_standard_expression(node->as<Expression>());
		if (node->is<LessEqualsExpression>()) return validate_standard_expression(node->as<Expression>());
		if (node->is<EqualsExpression>()) return validate_standard_expression(node->as<Expression>());
		if (node->is<NotEqualsExpression>()) return validate_standard_expression(node->as<Expression>());
		if (node->is<AssignmentExpression>()) return validate_assignment_expression(node->as<AssignmentExpression>());
		if (node->is<NegateChildrenExpression>()) return validate_standalone_expression(node->as<Expression>());

		if (node->is<VariableReference>()) return validate_variable_refernce(node->as<VariableReference>());

		if (node->is<YieldStatement>()) return validate_yield_statement(node->as<YieldStatement>());
		if (node->is<IfStatement>()) return validate_conditional_statement(node);
		if (node->is<UnlessStatement>()) return validate_conditional_statement(node);

		// Some nodes don't require validation
		if (node->is<TextLiteral>()) return;
		if (node->is<SelectorLiteral>()) return; // Maybe verify that the selector is valid
		if (node->is<NumericLiteral>()) return;
		if (node->is<FunctionCall>()) return;
		if (node->is<FunctionDeclaration>()) return;
		if (node->is<TemplateDeclaration>()) return;
		if (node->is<Program>()) return;
		if (node->is<RawStatement>()) return;

		// This needs to be at the bottom as a lot of nodes inherit from Expression
		if (node->is<Expression>()) return validate_standalone_expression(node->as<Expression>());

		fprintf(stderr, "%s\n", node->node_name());
		ErrorReporter::report_warning(node->token(), "I don't know how to validate this node");
	}
	void validate_standard_expression(Expression* node)
	{
		if (node->children().size() != 2) {
			ErrorReporter::report_error(node->token(), "This node must have exactly two children");
		}
		if (!node->children()[0]->is<Expression>()) {
			ErrorReporter::report_error(node->token(), "Both children of this node must be an Expression");
		}
		if (!node->children()[1]->is<Expression>()) {
			ErrorReporter::report_error(node->token(), "Both children of this node must be an Expression");
		}
	}
	void validate_assignment_expression(AssignmentExpression* node)
	{
		if (!node->identifier()) {
			ErrorReporter::report_error(node->token(), "An AssignmentExpression must have an identifier");
		}
		if (strcmp(node->identifier(), "__yield__") == 0) {
			ErrorReporter::report_error(node->token(), "Assignment of the __yield__ variable is not allowed. Please use the `yield` statement instead");
		}
		if (node->children().size() != 1) {
			ErrorReporter::report_error(node->token(), "This node must have exactly one child");
		}
		if (!node->children()[0]->is<Expression>()) {
			ErrorReporter::report_error(node->token(), "The child of an AssignmentExpression must be an Expression");
		}
	}
	void validate_variable_refernce(VariableReference* node)
	{
		if (!node->identifier()) {
			ErrorReporter::report_error(node->token(), "A VariableReference must have an identifier");
		}
		if (strcmp(node->identifier(), "__yield__") == 0) {
			ErrorReporter::report_error(node->token(), "Referencing the __yield__ variable is not allowed. Please use a function as an Expression instead");
		}
	}
	void validate_yield_statement(YieldStatement* node)
	{
		if (node->children().size() != 1) {
			ErrorReporter::report_error(node->token(), "This node must have exactly one child");
		}
		if (!node->children()[0]->is<Expression>()) {
			ErrorReporter::report_error(node->token(), "The child of an YieldStatement must be an Expression");
		}
	}
	void validate_standalone_expression(Expression* node)
	{
		if (node->children().size() != 1) {
			ErrorReporter::report_error(node->token(), "This node must have exactly one child");
		}
		if (!node->children()[0]->is<Expression>()) {
			ErrorReporter::report_error(node->token(), "The child of an YieldStatement must be an Expression");
		}
	}
	void validate_conditional_statement(ASTNode* node)
	{
		if (!node->is<IfStatement>() && !node->is<UnlessStatement>())
		{
			ErrorReporter::report_internal_error(node->token(), "Invalid node passed to validate_conditional_statement()");
		}
		if (node->children().size() <= 0) {
			ErrorReporter::report_error(node->token(), "This node needs to have at least one child");
		}
		ASTNode* first_child = node->children()[0];
		if (!first_child->is<ComparisonExpression>()) {
			ErrorReporter::report_error(node->token(), "The first child of a conditional statement node must be a ComparisonStatement");
		}
	}
};