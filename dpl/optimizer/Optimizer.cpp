#include "Optimizer.h"

ASTNode* Optimizer::run(ASTNode* node)
{
	if (node->is<Program>()) return optimize_children(node);
	if (node->is<FunctionDeclaration>()) return optimize_children(node);
	if (node->is<IfStatement>()) return optimize_children(node);
	if (node->is<UnlessStatement>()) return optimize_children(node);
	if (node->is<ComparisonExpression>()) return optimize_children(node);
	if (node->is<YieldStatement>()) return optimize_children(node);
	if (node->is<TemplateDeclaration>()) return node;
	if (node->is<Literal>()) return node;
	if (node->is<RawStatement>()) return node;
	if (node->is<FunctionCall>()) return node;
	if (node->is<VariableReference>()) return node;
	if (node->is<AssignmentExpression>())
	{
		if (can_be_optimized(node))
		{
			return optimize_children(node);
		}
		else
		{
			return node;
		}
	}
	if (node->is<AddExpression>())
	{
		if (can_be_optimized(node)) return optimize_math_expression(node->as<Expression>());
		else return optimize_children(node);
	}
	if (node->is<SubtractExpression>())
	{
		if (can_be_optimized(node)) return optimize_math_expression(node->as<Expression>());
		else return optimize_children(node);
	}
	if (node->is<MultiplyExpression>())
	{
		if (can_be_optimized(node)) return optimize_math_expression(node->as<Expression>());
		else return optimize_children(node);
	}
	if (node->is<DivideExpression>())
	{
		if (can_be_optimized(node)) return optimize_math_expression(node->as<Expression>());
		else return optimize_children(node);
	}
	if (node->is<ModuloExpression>())
	{
		if (can_be_optimized(node)) return optimize_math_expression(node->as<Expression>());
		else return optimize_children(node);
	}
	if (node->is<Expression>())
	{
		// FIXME: Find a better way to check it is just a plain Expression
		if (strcmp(node->node_name(), "Expression") == 0)
		{
			return node->children()[0];
		}
	}
	ErrorReporter::report_internal_error(node->token(), "I don't know how to optimize this node");
}

ASTNode* Optimizer::optimize_children(ASTNode* node)
{
	std::vector<ASTNode*> new_children{};
	for (ASTNode* child : node->children())
	{
		ASTNode* optimized_child = run(child);
		new_children.push_back(optimized_child);
	}
	node->clear_children();
	for (ASTNode* new_child : new_children)
	{
		node->add_child(new_child);
	}
	return node;
}

NumericLiteral* Optimizer::optimize_math_expression(Expression* expr)
{
	if (expr->is<NumericLiteral>()) return expr->as<NumericLiteral>();
	NumericLiteral* lit1 = optimize_math_expression(expr->children()[0]->as<Expression>());
	NumericLiteral* lit2 = optimize_math_expression(expr->children()[1]->as<Expression>());
	int value = 0;
	if (expr->is<AddExpression>())
	{
		value = lit1->value() + lit2->value();
	}
	else if (expr->is<SubtractExpression>())
	{
		value = lit1->value() - lit2->value();
	}
	else if (expr->is<MultiplyExpression>())
	{
		value = lit1->value() * lit2->value();
	}
	else if (expr->is<DivideExpression>())
	{
		value = lit1->value() / lit2->value();
	}
	else if (expr->is<ModuloExpression>())
	{
		value = lit1->value() % lit2->value();
	}
	else
	{
		ErrorReporter::report_internal_error(expr->token(), "I don't know how to optimize this mathematical expression");
	}
	NumericLiteral* ret = new NumericLiteral(value);
	return ret;
}

bool Optimizer::can_be_optimized(ASTNode* node)
{
	for (ASTNode* child : node->children())
	{
		if (!can_be_optimized(child)) return false;
	}
	if (node->is<Literal>()) return true;
	if (node->is<AssignmentExpression>()) return true;
	if (node->is<AddExpression>()) return true;
	if (node->is<SubtractExpression>()) return true;
	if (node->is<MultiplyExpression>()) return true;
	if (node->is<DivideExpression>()) return true;
	if (node->is<ModuloExpression>()) return true;
	return false;
}
