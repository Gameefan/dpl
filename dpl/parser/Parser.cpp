#include "Parser.h"
#include "AST.h"
#include <cassert>
#include "../utils/ErrorReporter.h"


ASTNode* Parser::run(std::vector<LexerToken> tokens)
{
	ParserState state = ParserState::Code;
	Program* program = new Program();
	ASTNode* target = program;

	size_t token_ptr = 0;

	ParserState argument_return_state = ParserState::Code;

	do {
		LexerToken* current_token = &tokens[token_ptr];

		if (current_token->type == LexerTokenType::EndOfFile) {
			break;
		}

		LexerToken* next_token = &tokens[token_ptr + 1];

		switch (state)
		{
		case ParserState::Code:
			if (current_token->type == LexerTokenType::VariableName) {
				state = ParserState::VariableOperation;
				token_ptr--;
				break;
			}
			if (current_token->type == LexerTokenType::Function) {
				create_new<FunctionDeclaration>(&target, current_token)->set_identifier(current_token->text_value);
				break;
			}
			if (current_token->type == LexerTokenType::Template) {
				create_new<TemplateDeclaration>(&target, current_token)->set_identifier(current_token->text_value);
				break;
			}
			if (current_token->type == LexerTokenType::ArgsStart) {
				state = ParserState::Arguments;
				argument_return_state = ParserState::Code;
				break;
			}
			if (current_token->type == LexerTokenType::BlockStart) {
				target = target->last_child();
				break;
			}
			if (current_token->type == LexerTokenType::FunctionCall) {
				create_new<FunctionCall>(&target, current_token)->set_callee(current_token->text_value);
				break;
			}
			if (current_token->type == LexerTokenType::BlockEnd) {
				go_up(&target);
				break;
			}
			if (current_token->type == LexerTokenType::RawData) {
				create_new<RawStatement>(&target, current_token)->set_value(current_token->text_value);
				if (target->parent()->is<TemplateDeclaration>()) {
					go_up(&target);
					go_up(&target);
					break;
				}
				if (next_token->type != LexerTokenType::ArgsStart)
				{
					go_up(&target);
				}
				break;
			}
			if (current_token->type == LexerTokenType::IfStatement) {
				create_new<IfStatement>(&target, current_token);
				break;
			}
			if (current_token->type == LexerTokenType::UnlessStatement) {
				create_new<UnlessStatement>(&target, current_token);
				break;
			}
			if (current_token->type == LexerTokenType::InlineStart) {
				create_new<Expression>(&target, current_token);
				state = ParserState::Expression;
				break;
			}
			if (current_token->type == LexerTokenType::InlineEnd) {
				go_up(&target);
				if (target->is<NegateChildrenExpression>()) {
					go_up(&target);
				}
				break;
			}
			if (current_token->type == LexerTokenType::YieldStatement) {
				create_new<YieldStatement>(&target, current_token);
				break;
			}
			if (current_token->type == LexerTokenType::Semicolon) {
				// We don't want to do that for stray semicolons
				if (!target->is<FunctionDeclaration>() && !target->is<IfStatement>() && !target->is<UnlessStatement>() && !target->is<Program>()) {
					go_up(&target);
				}
				break;
			}
			ErrorReporter::report_error(current_token, "Found unexpected token in Code state");
			break;
		case ParserState::VariableOperation:
			if (next_token->type == LexerTokenType::OpEquals) {
				create_new<AssignmentExpression>(&target, next_token)->set_identifier(current_token->text_value);
				state = ParserState::Expression;
				token_ptr++;
				break;
			}
			if (next_token->type == LexerTokenType::OpMultiply) {
				create_new<AssignmentExpression>(&target, next_token)->set_identifier(current_token->text_value);
				state = ParserState::Expression;
				token_ptr++;
				create_new<MultiplyExpression>(&target, next_token);
				create_new_dont_enter<VariableReference>(&target, current_token)->set_identifier(current_token->text_value);
				// TODO: Verify that the next next token is an OpEquals
				token_ptr++;
				break;
			}
			if (next_token->type == LexerTokenType::OpDivide) {
				create_new<AssignmentExpression>(&target, next_token)->set_identifier(current_token->text_value);
				state = ParserState::Expression;
				token_ptr++;
				create_new<DivideExpression>(&target, next_token);
				create_new_dont_enter<VariableReference>(&target, current_token)->set_identifier(current_token->text_value);
				// TODO: Verify that the next next token is an OpEquals
				token_ptr++;
				break;
			}
			if (next_token->type == LexerTokenType::OpAdd) {
				create_new<AssignmentExpression>(&target, next_token)->set_identifier(current_token->text_value);
				state = ParserState::Expression;
				token_ptr++;
				create_new<AddExpression>(&target, next_token);
				create_new_dont_enter<VariableReference>(&target, current_token)->set_identifier(current_token->text_value);
				// TODO: Verify that the next next token is an OpEquals
				token_ptr++;
				break;
			}
			if (next_token->type == LexerTokenType::OpSubtract) {
				create_new<AssignmentExpression>(&target, next_token)->set_identifier(current_token->text_value);
				state = ParserState::Expression;
				token_ptr++;
				create_new<SubtractExpression>(&target, next_token);
				create_new_dont_enter<VariableReference>(&target, current_token)->set_identifier(current_token->text_value);
				// TODO: Verify that the next next token is an OpEquals
				token_ptr++;
				break;
			}
			if (next_token->type == LexerTokenType::OpModulo) {
				create_new<AssignmentExpression>(&target, next_token)->set_identifier(current_token->text_value);
				state = ParserState::Expression;
				token_ptr++;
				create_new<ModuloExpression>(&target, next_token);
				create_new_dont_enter<VariableReference>(&target, current_token)->set_identifier(current_token->text_value);
				// TODO: Verify that the next next token is an OpEquals
				token_ptr++;
				break;
			}
			ErrorReporter::report_error(next_token, "Invalid variable operation");
			break;
		case ParserState::Expression:
			if (current_token->type == LexerTokenType::NumericLiteral)
			{
				create_new_dont_enter<NumericLiteral>(&target, current_token)->set_value(current_token->numeric_value);
				break;
			}
			if (current_token->type == LexerTokenType::VariableName)
			{
				create_new_dont_enter<VariableReference>(&target, current_token)->set_identifier(current_token->text_value);
				break;
			}
			if (current_token->type == LexerTokenType::OpAdd)
			{
				ASTNode* lhs = target;
				if (lhs->is<AssignmentExpression>()) {
					ErrorReporter::assert_or_error(target->children().size() == 1, current_token, "An AssignmentExpression must have exactly one child");
					assert(target->children().size() == 1);
					target = target->last_child();
					lhs = target;
				}
				go_up(&target);
				target->remove_last_child();
				create_new<AddExpression>(&target, current_token);
				target->add_child(lhs);
				break;
			}
			if (current_token->type == LexerTokenType::OpSubtract)
			{
				ASTNode* lhs = target;
				if (lhs->is<AssignmentExpression>()) {
					ErrorReporter::assert_or_error(target->children().size() == 1, current_token, "An AssignmentExpression must have exactly one child");
					assert(target->children().size() == 1);
					target = target->last_child();
					lhs = target;
				}
				go_up(&target);
				target->remove_last_child();
				create_new<SubtractExpression>(&target, current_token);
				target->add_child(lhs);
				break;
			}
			if (current_token->type == LexerTokenType::OpMultiply)
			{
				ASTNode* lhs = target;
				if (lhs->is<AssignmentExpression>()) {
					ErrorReporter::assert_or_error(target->children().size() == 1, current_token, "An AssignmentExpression must have exactly one child");
					assert(target->children().size() == 1);
					target = target->last_child();
					lhs = target;
				}
				go_up(&target);
				target->remove_last_child();
				create_new<MultiplyExpression>(&target, current_token);
				target->add_child(lhs);
				break;
			}
			if (current_token->type == LexerTokenType::OpDivide)
			{
				ASTNode* lhs = target;
				if (lhs->is<AssignmentExpression>()) {
					ErrorReporter::assert_or_error(target->children().size() == 1, current_token, "An AssignmentExpression must have exactly one child");
					assert(target->children().size() == 1);
					target = target->last_child();
					lhs = target;
				}
				go_up(&target);
				target->remove_last_child();
				create_new<DivideExpression>(&target, current_token);
				target->add_child(lhs);
				break;
			}
			if (current_token->type == LexerTokenType::OpModulo)
			{
				ASTNode* lhs = target;
				if (lhs->is<AssignmentExpression>()) {
					ErrorReporter::assert_or_error(target->children().size() == 1, current_token, "An AssignmentExpression must have exactly one child");
					assert(target->children().size() == 1);
					target = target->last_child();
					lhs = target;
				}
				go_up(&target);
				target->remove_last_child();
				create_new<ModuloExpression>(&target, current_token);
				target->add_child(lhs);
				break;
			}
			if (current_token->type == LexerTokenType::OpLess)
			{
				if (next_token->type == LexerTokenType::OpEquals)
				{
					ASTNode* lhs = target;
					go_up(&target);
					target->remove_last_child();
					create_new<LessEqualsExpression>(&target, current_token);
					target->add_child(lhs);
					token_ptr++;
					break;
				}
				else
				{
					ASTNode* lhs = target;
					go_up(&target);
					target->remove_last_child();
					create_new<LessExpression>(&target, current_token);
					target->add_child(lhs);
					break;
				}
			}
			if (current_token->type == LexerTokenType::OpEquals)
			{
				if (next_token->type != LexerTokenType::OpEquals) {
					ErrorReporter::report_error(next_token, "Unexpected token after equals sign");
				}
				ASTNode* lhs = target;
				go_up(&target);
				target->remove_last_child();
				create_new<EqualsExpression>(&target, current_token);
				target->add_child(lhs);
				token_ptr++;
				break;
			}
			if (current_token->type == LexerTokenType::OpMore)
			{
				if (next_token->type == LexerTokenType::OpEquals)
				{
					ASTNode* lhs = target;
					go_up(&target);
					target->remove_last_child();
					create_new<MoreEqualsExpression>(&target, current_token);
					target->add_child(lhs);
					token_ptr++;
					break;
				}
				else
				{
					ASTNode* lhs = target;
					go_up(&target);
					target->remove_last_child();
					create_new<MoreExpression>(&target, current_token);
					target->add_child(lhs);
					break;
				}
			}
			if (current_token->type == LexerTokenType::OpNot)
			{
				if (next_token->type == LexerTokenType::OpEquals) {
					ASTNode* lhs = target;
					go_up(&target);
					target->remove_last_child();
					create_new<NotEqualsExpression>(&target, current_token);
					target->add_child(lhs);
					token_ptr++;
					break;
				}
				else if (next_token->type == LexerTokenType::InlineStart) {
					create_new<NegateChildrenExpression>(&target, current_token);
					break;
				}
				else {
					ErrorReporter::report_error(current_token, "Unexpected token after ! operator");
				}
			}
			if (current_token->type == LexerTokenType::InlineStart) {
				create_new<Expression>(&target, current_token);
				break;
			}
			if (current_token->type == LexerTokenType::FunctionCall) {
				create_new<FunctionCall>(&target, current_token)->set_callee(current_token->text_value);
				break;
			}
			if (current_token->type == LexerTokenType::ArgsStart) {
				state = ParserState::Arguments;
				argument_return_state = ParserState::Expression;
				break;
			}
			// This is the end
			go_up(&target);
			if (target->is<AssignmentExpression>()) {
				go_up(&target);
			}
			state = ParserState::Code;
			token_ptr--;
			break;
		case ParserState::Arguments:
			if (current_token->type == LexerTokenType::ArgsEnd) {
				state = argument_return_state;
				go_up(&target);
				break;
			}
			if (target->is<FunctionDeclaration>()) {
				if (current_token->type == LexerTokenType::VariableName) {
					target->as<FunctionDeclaration>()->add_param(current_token->text_value);
					break;
				}
				ErrorReporter::report_error(current_token, "Function declaration argument must be a variable identifier");
			}
			else if (target->is<FunctionCall>()) {
				if (current_token->type == LexerTokenType::TextLiteral) {
					target->as<FunctionCall>()->add_param({ nullptr, new TextLiteral(current_token->text_value) });
					break;
				}
				if (current_token->type == LexerTokenType::NumericLiteral) {
					target->as<FunctionCall>()->add_param({ nullptr, new NumericLiteral(current_token->numeric_value) });
					break;
				}
				if (current_token->type == LexerTokenType::SelectorLiteral) {
					target->as<FunctionCall>()->add_param({ nullptr, new SelectorLiteral(current_token->text_value) });
					break;
				}
				if (current_token->type == LexerTokenType::VariableName) {
					target->as<FunctionCall>()->add_param({ current_token->text_value, nullptr });
					break;
				}
				ErrorReporter::report_error(current_token, "Unexpected token in function call arguments");
			}
			else if (target->is<RawStatement>()) {
				if (current_token->type == LexerTokenType::TextLiteral) {
					target->as<RawStatement>()->add_param({ nullptr, new TextLiteral(current_token->text_value) });
					break;
				}
				if (current_token->type == LexerTokenType::NumericLiteral) {
					target->as<RawStatement>()->add_param({ nullptr, new NumericLiteral(current_token->numeric_value) });
					break;
				}
				if (current_token->type == LexerTokenType::SelectorLiteral) {
					target->as<RawStatement>()->add_param({ nullptr, new SelectorLiteral(current_token->text_value) });
					break;
				}
				if (current_token->type == LexerTokenType::VariableName) {
					target->as<RawStatement>()->add_param({ current_token->text_value, nullptr });
					break;
				}
				ErrorReporter::report_error(current_token, "Unexpected token in raw statement arguments");
			}
			else {
				ErrorReporter::report_error(current_token, "Invalid target while parsing arguments");
			}
			break;
		default:
			ErrorReporter::report_error(current_token, "Unexpected parser state");
			abort();
			break;
		}
	} while (++token_ptr < tokens.size());

	// Ensure this is the root node
	ErrorReporter::assert_or_internal_error(!target->parent(), target->token(), "The target node should be the root node at the end of parsing");
	assert(!target->parent());

	return target;
}

void Parser::go_up(ASTNode** current_target)
{
	*current_target = (*current_target)->parent();
}

template<typename T>
T* Parser::create_new(ASTNode** current_target, LexerToken* token)
{
	static_assert(std::is_base_of<ASTNode, T>::value);
	ErrorReporter::assert_or_internal_error(current_target, token, "A target must be provided to Parser::create_new()");
	assert(current_target);
	ErrorReporter::assert_or_internal_error(*current_target, token, "A target must be provided to Parser::create_new()");
	assert(*current_target);
	T* new_target = new T();
	// We need to create a copy of the LexerToken here as the original one is destroyed when std::vector<LexerToken> goes out of scope when Parser::run() returns
	((ASTNode*)new_target)->set_token(new LexerToken(*token));
	(*current_target)->add_child(new_target);
	*current_target = new_target;
	return new_target;
}
