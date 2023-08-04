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
	if (node->is<AddExpression>()) return optimize_math_expression(node->as<Expression>());
	if (node->is<SubtractExpression>()) return optimize_math_expression(node->as<Expression>());
	if (node->is<MultiplyExpression>()) return optimize_math_expression(node->as<Expression>());
	if (node->is<DivideExpression>()) return optimize_math_expression(node->as<Expression>());
	if (node->is<ModuloExpression>()) return optimize_math_expression(node->as<Expression>());
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

Expression* Optimizer::optimize_math_expression(Expression* expr)
{
	if (expr->children().size() < 2)
	{
		return expr;
	}

	if (expr->is<NumericLiteral>()) return expr->as<NumericLiteral>();
	Expression* lit1 = optimize_math_expression(expr->children()[0]->as<Expression>());
	Expression* lit2 = optimize_math_expression(expr->children()[1]->as<Expression>());

	// Both expressions are literals
	if (lit1->is<NumericLiteral>() && lit2->is<NumericLiteral>()) {
		NumericLiteral* num1 = dynamic_cast<NumericLiteral*>(lit1);
		NumericLiteral* num2 = dynamic_cast<NumericLiteral*>(lit2);
		int value = 0;
		if (expr->is<AddExpression>())
		{
			value = num1->value() + num2->value();
		}
		else if (expr->is<SubtractExpression>())
		{
			value = num1->value() - num2->value();
		}
		else if (expr->is<MultiplyExpression>())
		{
			value = num1->value() * num2->value();
		}
		else if (expr->is<DivideExpression>())
		{
			value = num1->value() / num2->value();
		}
		else if (expr->is<ModuloExpression>())
		{
			value = num1->value() % num2->value();
		}
		else
		{
			ErrorReporter::report_internal_error(expr->token(), "I don't know how to optimize this mathematical expression");
		}
		NumericLiteral* ret = new NumericLiteral(value);
		return ret;
	}

	// At least one of the expressions isn't a literal
	if (expr->is<AddExpression>())
	{
		AddExpression* ret = new AddExpression();
		ret->add_child(lit1);
		ret->add_child(lit2);
		return ret;
	}
	else if (expr->is<SubtractExpression>())
	{
		SubtractExpression* ret = new SubtractExpression();
		ret->add_child(lit1);
		ret->add_child(lit2);
		return ret;
	}
	else if (expr->is<MultiplyExpression>())
	{
		MultiplyExpression* ret = new MultiplyExpression();
		ret->add_child(lit1);
		ret->add_child(lit2);
		return ret;
	}
	else if (expr->is<DivideExpression>())
	{
		DivideExpression* ret = new DivideExpression();
		ret->add_child(lit1);
		ret->add_child(lit2);
		return ret;
	}
	else if (expr->is<ModuloExpression>())
	{
		ModuloExpression* ret = new ModuloExpression();
		ret->add_child(lit1);
		ret->add_child(lit2);
		return ret;
	}
	else
	{
		ErrorReporter::report_internal_error(expr->token(), "I don't know how to optimize this mathematical expression");
	}
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
