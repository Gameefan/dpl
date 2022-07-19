#pragma once
#include "../parser/AST.h"
#include "../utils/ErrorReporter.h"

class Optimizer {
public:
	ASTNode* run(ASTNode* node);
private:
	ASTNode* optimize_children(ASTNode* node);
	NumericLiteral* optimize_math_expression(Expression* expr);
	bool can_be_optimized(ASTNode* node);
};