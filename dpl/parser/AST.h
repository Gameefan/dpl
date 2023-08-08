#pragma once
#include "ASTNode.h"
#include <cstdio>

class Program : public ASTNode {
public:
	virtual const char* node_name() override { return "Program"; }
private:
};

class Expression : public ASTNode {
public:
	virtual const char* node_name() override { return "Expression"; }
private:
};

class ComparisonExpression : public Expression {
public:
	virtual const char* node_name() override { return "ComparisonExpression"; }
private:
};

class AssignmentExpression : public Expression {
public:
	virtual const char* node_name() override { return "AssignmentExpression"; }
	const char* identifier() { return m_identifier; }
	void set_identifier(const char* value) { m_identifier = value; }
private:
	const char* m_identifier;
	virtual void print_custom_data() override
	{
		printf("ident=%s", m_identifier);
	}
};

// This node must have two children, both of type Expression
class MoreExpression : public ComparisonExpression {
public:
	virtual const char* node_name() override { return "MoreExpression"; }
private:
};

// This node must have two children, both of type Expression
class MoreEqualsExpression : public ComparisonExpression {
public:
	virtual const char* node_name() override { return "MoreEqualsExpression"; }
private:
};

// This node must have two children, both of type Expression
class LessExpression : public ComparisonExpression {
public:
	virtual const char* node_name() override { return "LessExpression"; }
private:
};

// This node must have two children, both of type Expression
class LessEqualsExpression : public ComparisonExpression {
public:
	virtual const char* node_name() override { return "LessEqualsExpression"; }
private:
};

// This node must have two children, both of type Expression
class EqualsExpression : public ComparisonExpression {
public:
	virtual const char* node_name() override { return "EqualsExpression"; }
private:
};

// This node must have two children, both of type Expression
class NotEqualsExpression : public ComparisonExpression {
public:
	virtual const char* node_name() override { return "NotEqualsExpression"; }
private:
};

// This node must have exactly one child of type Expression
class NegateChildrenExpression : public ComparisonExpression {
public:
	virtual const char* node_name() override { return "NegateChildrenExpression"; }
private:
};

// This node must have two children, both of type Expression
class AddExpression : public Expression {
public:
	virtual const char* node_name() override { return "AddExpression"; }
private:
};

// This node must have two children, both of type Expression
class SubtractExpression : public Expression {
public:
	virtual const char* node_name() override { return "SubtractExpression"; }
private:
};

// This node must have two children, both of type Expression
class MultiplyExpression : public Expression {
public:
	virtual const char* node_name() override { return "MultiplyExpression"; }
private:
};

// This node must have two children, both of type Expression
class DivideExpression : public Expression {
public:
	virtual const char* node_name() override { return "DivideExpression"; }
private:
};

// This node must have two children, both of type Expression
class ModuloExpression : public Expression {
public:
	virtual const char* node_name() override { return "ModuloExpression"; }
private:
};

class VariableReference : public Expression {
public:
	virtual const char* node_name() override { return "VariableReference"; }
	const char* identifier() { return m_identifier; }
	void set_identifier(const char* value) { m_identifier = value; }
private:
	const char* m_identifier{};
	virtual void print_custom_data() override
	{
		printf("ident=%s", m_identifier);
	}
};

class FunctionDeclaration : public ASTNode {
public:
	virtual const char* node_name() override { return "FunctionDeclaration"; }
	const char* identifier() { return m_identifier; }
	void set_identifier(const char* value) { m_identifier = value; }
	void add_param(const char* node) { m_params.push_back(node); }
	std::vector<const char*> params() { return m_params; };
private:
	const char* m_identifier{};
	std::vector<const char*> m_params{};
	virtual void print_custom_data() override
	{
		printf("ident=%s params=%llu", m_identifier, m_params.size());
	}
};

class TemplateDeclaration : public ASTNode {
public:
	virtual const char* node_name() override { return "TemplateDeclaration"; }
	const char* identifier() { return m_identifier; }
	void set_identifier(const char* value) { m_identifier = value; }
private:
	const char* m_identifier{};
	virtual void print_custom_data() override
	{
		printf("ident=%s", m_identifier);
	}
};

class Literal;

class FunctionCall : public Expression {
public:
	virtual const char* node_name() override { return "FunctionCall"; }
	const char* callee() { return m_callee; }
	void set_callee(const char* value) { m_callee= value; }
	struct Param {
		const char* identifier;
		Literal* literal;
	};
	void add_param(Param node) { m_params.push_back(node); }
	std::vector<Param> params() { return m_params; };
private:
	const char* m_callee;
	std::vector<Param> m_params{};
	virtual void print_custom_data() override
	{
		printf("callee=%s, params=%llu", m_callee, m_params.size());
	}
};

class Literal : public Expression {
public:
	virtual const char* node_name() override { return "Literal"; }
private:
};

class TextLiteral : public Literal {
public:
	virtual const char* node_name() override { return "TextLiteral"; }
	const char* value() { return m_value; }
	void set_value(const char* value) { m_value = value; }
	TextLiteral() {};
	TextLiteral(const char* value)
		: m_value(value)
	{}
private:
	const char* m_value;
	virtual void print_custom_data() override
	{
		printf("value=%s", m_value);
	}
};

class NumericLiteral : public Literal {
public:
	virtual const char* node_name() override { return "NumericLiteral"; }
	int value() { return m_value; }
	void set_value(int value) { m_value = value; }
	NumericLiteral() {};
	NumericLiteral(int value)
		: m_value(value)
	{}
private:
	int m_value;
	virtual void print_custom_data() override
	{
		printf("value=%d", m_value);
	}
};

class SelectorLiteral : public Literal {
public:
	virtual const char* node_name() override { return "SelectorLiteral"; }
	const char* value() { return m_value; }
	void set_value(const char* value) { m_value = value; }
	SelectorLiteral() {};
	SelectorLiteral(const char* value)
		: m_value(value)
	{}
private:
	const char* m_value;
	virtual void print_custom_data() override
	{
		printf("value=%s", m_value);
	}
};

class RawStatement : public ASTNode {
public:
	virtual const char* node_name() override { return "RawStatement"; }
	const char* value() { return m_value; }
	void set_value(const char* value) { m_value = value; }
	struct Param {
		const char* identifier;
		Literal* literal;
	};
	void add_param(Param node) { m_params.push_back(node); }
	std::vector<Param> params() { return m_params; };
	void clear_params() { m_params.clear(); }
private:
	const char* m_value;
	std::vector<Param> m_params{};
	virtual void print_custom_data() override
	{
		printf("params=%llu", m_params.size());
	}
};

// The first child of the IfStatement must be a Expression
class IfStatement : public ASTNode {
public:
	virtual const char* node_name() override { return "IfStatement"; }
private:
};

// The first child of the UnlessStatement must be a Expression
class UnlessStatement : public ASTNode {
public:
	virtual const char* node_name() override { return "UnlessStatement"; }
private:
};

// The first child of the YieldStatement must be a Expression
class YieldStatement : public ASTNode {
public:
	virtual const char* node_name() override { return "YieldStatement"; }
private:
};
