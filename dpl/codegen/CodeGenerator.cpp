#include "CodeGenerator.h"
#include <cassert>
#include <cctype>
#include "../utils/ErrorReporter.h"

// Inserts everything from B into A
template<typename T>
static void extend_vector(std::vector<T>* a, std::vector<T>* b)
{
    a->insert(a->end(), b->begin(), b->end());
}

std::vector<CodeGeneratorResult> CodeGenerator::run(Program* root_node)
{
    std::vector<CodeGeneratorResult> results;

    results.push_back(prepare_global_variables(root_node->as<Program>()));

    m_functions = prepare_functions(root_node->as<Program>());

    std::vector<CodeGeneratorResult> standard_files = prepare_standard_files();
    extend_vector<CodeGeneratorResult>(&results, &standard_files);

    std::vector<CodeGeneratorResult> functions = generate_functions(root_node->as<Program>());
    extend_vector<CodeGeneratorResult>(&results, &functions);

    prepare_notice(results);

    return results;
}

CodeGeneratorResult CodeGenerator::prepare_global_variables(Program* program)
{
    std::vector<const char*> lines;

    lines.push_back("scoreboard objectives remove dpl_variables");
    lines.push_back("scoreboard objectives remove temp_variables");

    lines.push_back("scoreboard objectives add dpl_variables dummy");
    lines.push_back("scoreboard objectives add temp_variables dummy");

    for (ASTNode* child : program->children())
    {
        if (child->is<AssignmentExpression>()) {
            AssignmentExpression* ass = child->as<AssignmentExpression>();
            char* temp = (char*)malloc(512);
            ErrorReporter::assert_or_internal_error(temp, child->token(), "Failed to malloc");
            assert(temp);
            assert(child->children().size() > 0);
            EvaluationResult eval = evaluate_math_expression(ass->children()[0]->as<Expression>());
            extend_vector<const char*>(&lines, &eval.lines);
            sprintf(temp, "scoreboard players operation %s dpl_variables = %s temp_variables", ass->identifier(), eval.result_identifier);
            lines.push_back(temp);
        }
    }
    return { "data/dpl/functions/init.mcfunction", lines };
}

EvaluationResult CodeGenerator::evaluate_math_expression(Expression* expr)
{
    const char* temporary_var = create_temporary_variable_identifier();
    if (expr->is<NumericLiteral>()) {
        char* temp = (char*)malloc(64);
        ErrorReporter::assert_or_internal_error(temp, expr->token(), "Failed to malloc");
        assert(temp);
        sprintf(temp, "scoreboard players set %s temp_variables %d", temporary_var, expr->as<NumericLiteral>()->value());
        return { { temp }, temporary_var };
    }
    if (expr->is<AddExpression>()) {
        AddExpression* add_expr = expr->as<AddExpression>();
        ErrorReporter::assert_or_internal_error(add_expr->children().size() == 2, expr->token(), "AddExpression has invalid amount of children (Validator failed)");
        assert(add_expr->children().size() == 2);
        EvaluationResult res1 = evaluate_math_expression(add_expr->children()[0]->as<Expression>());
        EvaluationResult res2 = evaluate_math_expression(add_expr->children()[1]->as<Expression>());
        std::vector<const char*> lines{};
        extend_vector<const char*>(&lines, &res1.lines);
        extend_vector<const char*>(&lines, &res2.lines);

        const char* temp_var_name = create_temporary_variable_identifier();

        char* temp1 = (char*)malloc(512);
        ErrorReporter::assert_or_internal_error(temp1, expr->token(), "Failed to malloc");
        assert(temp1);
        sprintf(temp1, "scoreboard players operation %s temp_variables = %s temp_variables", temp_var_name, res1.result_identifier);

        char* temp2 = (char*)malloc(512);
        ErrorReporter::assert_or_internal_error(temp2, expr->token(), "Failed to malloc");
        assert(temp2);
        sprintf(temp2, "scoreboard players operation %s temp_variables += %s temp_variables", temp_var_name, res2.result_identifier);

        lines.push_back(temp1);
        lines.push_back(temp2);

        return { lines, temp_var_name };
    }
    if (expr->is<SubtractExpression>()) {
        SubtractExpression* sub_expr = expr->as<SubtractExpression>();
        ErrorReporter::assert_or_internal_error(sub_expr->children().size() == 2, expr->token(), "SubtractExpression has invalid amount of children (Validator failed)");
        assert(sub_expr->children().size() == 2);
        EvaluationResult res1 = evaluate_math_expression(sub_expr->children()[0]->as<Expression>());
        EvaluationResult res2 = evaluate_math_expression(sub_expr->children()[1]->as<Expression>());
        std::vector<const char*> lines{};
        extend_vector<const char*>(&lines, &res1.lines);
        extend_vector<const char*>(&lines, &res2.lines);

        const char* temp_var_name = create_temporary_variable_identifier();

        char* temp1 = (char*)malloc(512);
        ErrorReporter::assert_or_internal_error(temp1, expr->token(), "Failed to malloc");
        assert(temp1);
        sprintf(temp1, "scoreboard players operation %s temp_variables = %s temp_variables", temp_var_name, res1.result_identifier);

        char* temp2 = (char*)malloc(512);
        ErrorReporter::assert_or_internal_error(temp2, expr->token(), "Failed to malloc");
        assert(temp2);
        sprintf(temp2, "scoreboard players operation %s temp_variables -= %s temp_variables", temp_var_name, res2.result_identifier);

        lines.push_back(temp1);
        lines.push_back(temp2);

        return { lines, temp_var_name };
    }
    if (expr->is<MultiplyExpression>()) {
        MultiplyExpression* mul_expr = expr->as<MultiplyExpression>();
        ErrorReporter::assert_or_internal_error(mul_expr->children().size() == 2, expr->token(), "MultiplyExpression has invalid amount of children (Validator failed)");
        assert(mul_expr->children().size() == 2);
        EvaluationResult res1 = evaluate_math_expression(mul_expr->children()[0]->as<Expression>());
        EvaluationResult res2 = evaluate_math_expression(mul_expr->children()[1]->as<Expression>());
        std::vector<const char*> lines{};
        extend_vector<const char*>(&lines, &res1.lines);
        extend_vector<const char*>(&lines, &res2.lines);

        const char* temp_var_name = create_temporary_variable_identifier();

        char* temp1 = (char*)malloc(512);
        ErrorReporter::assert_or_internal_error(temp1, expr->token(), "Failed to malloc");
        assert(temp1);
        sprintf(temp1, "scoreboard players operation %s temp_variables = %s temp_variables", temp_var_name, res1.result_identifier);

        char* temp2 = (char*)malloc(512);
        ErrorReporter::assert_or_internal_error(temp2, expr->token(), "Failed to malloc");
        assert(temp2);
        sprintf(temp2, "scoreboard players operation %s temp_variables *= %s temp_variables", temp_var_name, res2.result_identifier);

        lines.push_back(temp1);
        lines.push_back(temp2);

        return { lines, temp_var_name };
    }
    if (expr->is<DivideExpression>()) {
        DivideExpression* div_expr = expr->as<DivideExpression>();
        ErrorReporter::assert_or_internal_error(div_expr->children().size() == 2, expr->token(), "DivideExpression has invalid amount of children (Validator failed)");
        assert(div_expr->children().size() == 2);
        EvaluationResult res1 = evaluate_math_expression(div_expr->children()[0]->as<Expression>());
        EvaluationResult res2 = evaluate_math_expression(div_expr->children()[1]->as<Expression>());
        std::vector<const char*> lines{};
        extend_vector<const char*>(&lines, &res1.lines);
        extend_vector<const char*>(&lines, &res2.lines);

        const char* temp_var_name = create_temporary_variable_identifier();

        char* temp1 = (char*)malloc(512);
        ErrorReporter::assert_or_internal_error(temp1, expr->token(), "Failed to malloc");
        assert(temp1);
        sprintf(temp1, "scoreboard players operation %s temp_variables = %s temp_variables", temp_var_name, res1.result_identifier);

        char* temp2 = (char*)malloc(512);
        ErrorReporter::assert_or_internal_error(temp2, expr->token(), "Failed to malloc");
        assert(temp2);
        sprintf(temp2, "scoreboard players operation %s temp_variables /= %s temp_variables", temp_var_name, res2.result_identifier);

        lines.push_back(temp1);
        lines.push_back(temp2);

        return { lines, temp_var_name };
    }
    if (expr->is<ModuloExpression>()) {
        ModuloExpression* mod_expr = expr->as<ModuloExpression>();
        ErrorReporter::assert_or_internal_error(mod_expr->children().size() == 2, expr->token(), "ModuloExpression has invalid amount of children (Validator failed)");
        assert(mod_expr->children().size() == 2);
        EvaluationResult res1 = evaluate_math_expression(mod_expr->children()[0]->as<Expression>());
        EvaluationResult res2 = evaluate_math_expression(mod_expr->children()[1]->as<Expression>());
        std::vector<const char*> lines{};
        extend_vector<const char*>(&lines, &res1.lines);
        extend_vector<const char*>(&lines, &res2.lines);

        const char* temp_var_name = create_temporary_variable_identifier();

        char* temp1 = (char*)malloc(512);
        ErrorReporter::assert_or_internal_error(temp1, expr->token(), "Failed to malloc");
        assert(temp1);
        sprintf(temp1, "scoreboard players operation %s temp_variables = %s temp_variables", temp_var_name, res1.result_identifier);

        char* temp2 = (char*)malloc(512);
        ErrorReporter::assert_or_internal_error(temp2, expr->token(), "Failed to malloc");
        assert(temp2);
        sprintf(temp2, "scoreboard players operation %s temp_variables %= %s temp_variables", temp_var_name, res2.result_identifier);

        lines.push_back(temp1);
        lines.push_back(temp2);

        return { lines, temp_var_name };
    }

    if (expr->is<MoreExpression>()) {
        MoreExpression* more_expr = expr->as<MoreExpression>();
        ErrorReporter::assert_or_internal_error(more_expr->children().size() == 2, expr->token(), "MoreExpression has invalid amount of children (Validator failed)");
        assert(more_expr->children().size() == 2);
        EvaluationResult res1 = evaluate_math_expression(more_expr->children()[0]->as<Expression>());
        EvaluationResult res2 = evaluate_math_expression(more_expr->children()[1]->as<Expression>());
        std::vector<const char*> lines{};
        extend_vector<const char*>(&lines, &res1.lines);
        extend_vector<const char*>(&lines, &res2.lines);

        const char* temp_var_name = create_temporary_variable_identifier();

        char* temp = (char*)malloc(512);
        ErrorReporter::assert_or_internal_error(temp, expr->token(), "Failed to malloc");
        assert(temp);
        sprintf(temp, "execute if score %s temp_variables > %s temp_variables run scoreboard players set %s temp_variables 1", res1.result_identifier, res2.result_identifier, temp_var_name);

        lines.push_back(clear_temp_var(temp_var_name));
        lines.push_back(temp);

        return { lines, temp_var_name };
    }

    if (expr->is<MoreEqualsExpression>()) {
        MoreEqualsExpression* more_eq_expr = expr->as<MoreEqualsExpression>();
        ErrorReporter::assert_or_internal_error(more_eq_expr->children().size() == 2, expr->token(), "MoreExpression has invalid amount of children (Validator failed)");
        assert(more_eq_expr->children().size() == 2);
        EvaluationResult res1 = evaluate_math_expression(more_eq_expr->children()[0]->as<Expression>());
        EvaluationResult res2 = evaluate_math_expression(more_eq_expr->children()[1]->as<Expression>());
        std::vector<const char*> lines{};
        extend_vector<const char*>(&lines, &res1.lines);
        extend_vector<const char*>(&lines, &res2.lines);

        const char* temp_var_name = create_temporary_variable_identifier();

        char* temp = (char*)malloc(512);
        ErrorReporter::assert_or_internal_error(temp, expr->token(), "Failed to malloc");
        assert(temp);
        sprintf(temp, "execute if score %s temp_variables >= %s temp_variables run scoreboard players set %s temp_variables 1", res1.result_identifier, res2.result_identifier, temp_var_name);

        lines.push_back(clear_temp_var(temp_var_name));
        lines.push_back(temp);

        return { lines, temp_var_name };
    }

    if (expr->is<LessExpression>()) {
        LessExpression* less_expr = expr->as<LessExpression>();
        ErrorReporter::assert_or_internal_error(less_expr->children().size() == 2, expr->token(), "LessExpression has invalid amount of children (Validator failed)");
        assert(less_expr->children().size() == 2);
        EvaluationResult res1 = evaluate_math_expression(less_expr->children()[0]->as<Expression>());
        EvaluationResult res2 = evaluate_math_expression(less_expr->children()[1]->as<Expression>());
        std::vector<const char*> lines{};
        extend_vector<const char*>(&lines, &res1.lines);
        extend_vector<const char*>(&lines, &res2.lines);

        const char* temp_var_name = create_temporary_variable_identifier();

        char* temp = (char*)malloc(512);
        ErrorReporter::assert_or_internal_error(temp, expr->token(), "Failed to malloc");
        assert(temp);
        sprintf(temp, "execute if score %s temp_variables < %s temp_variables run scoreboard players set %s temp_variables 1", res1.result_identifier, res2.result_identifier, temp_var_name);

        lines.push_back(clear_temp_var(temp_var_name));
        lines.push_back(temp);

        return { lines, temp_var_name };
    }

    if (expr->is<LessEqualsExpression>()) {
        LessEqualsExpression* less_eq_expr = expr->as<LessEqualsExpression>();
        ErrorReporter::assert_or_internal_error(less_eq_expr->children().size() == 2, expr->token(), "LessExpression has invalid amount of children (Validator failed)");
        assert(less_eq_expr->children().size() == 2);
        EvaluationResult res1 = evaluate_math_expression(less_eq_expr->children()[0]->as<Expression>());
        EvaluationResult res2 = evaluate_math_expression(less_eq_expr->children()[1]->as<Expression>());
        std::vector<const char*> lines{};
        extend_vector<const char*>(&lines, &res1.lines);
        extend_vector<const char*>(&lines, &res2.lines);

        const char* temp_var_name = create_temporary_variable_identifier();

        char* temp = (char*)malloc(512);
        ErrorReporter::assert_or_internal_error(temp, expr->token(), "Failed to malloc");
        assert(temp);
        sprintf(temp, "execute if score %s temp_variables <= %s temp_variables run scoreboard players set %s temp_variables 1", res1.result_identifier, res2.result_identifier, temp_var_name);

        lines.push_back(clear_temp_var(temp_var_name));
        lines.push_back(temp);

        return { lines, temp_var_name };
    }

    if (expr->is<EqualsExpression>()) {
        EqualsExpression* eq_expr = expr->as<EqualsExpression>();
        ErrorReporter::assert_or_internal_error(eq_expr->children().size() == 2, expr->token(), "EqualsExpression has invalid amount of children (Validator failed)");
        assert(eq_expr->children().size() == 2);
        EvaluationResult res1 = evaluate_math_expression(eq_expr->children()[0]->as<Expression>());
        EvaluationResult res2 = evaluate_math_expression(eq_expr->children()[1]->as<Expression>());
        std::vector<const char*> lines{};
        extend_vector<const char*>(&lines, &res1.lines);
        extend_vector<const char*>(&lines, &res2.lines);

        const char* temp_var_name = create_temporary_variable_identifier();

        char* temp = (char*)malloc(512);
        ErrorReporter::assert_or_internal_error(temp, expr->token(), "Failed to malloc");
        assert(temp);
        sprintf(temp, "execute if score %s temp_variables = %s temp_variables run scoreboard players set %s temp_variables 1", res1.result_identifier, res2.result_identifier, temp_var_name);

        lines.push_back(clear_temp_var(temp_var_name));
        lines.push_back(temp);

        return { lines, temp_var_name };
    }

    if (expr->is<NotEqualsExpression>()) {
        NotEqualsExpression* neq_expr = expr->as<NotEqualsExpression>();
        ErrorReporter::assert_or_internal_error(neq_expr->children().size() == 2, expr->token(), "EqualsExpression has invalid amount of children (Validator failed)");
        assert(neq_expr->children().size() == 2);
        EvaluationResult res1 = evaluate_math_expression(neq_expr->children()[0]->as<Expression>());
        EvaluationResult res2 = evaluate_math_expression(neq_expr->children()[1]->as<Expression>());
        std::vector<const char*> lines{};
        extend_vector<const char*>(&lines, &res1.lines);
        extend_vector<const char*>(&lines, &res2.lines);

        const char* temp_var_name = create_temporary_variable_identifier();

        char* temp = (char*)malloc(512);
        ErrorReporter::assert_or_internal_error(temp, expr->token(), "Failed to malloc");
        assert(temp);
        sprintf(temp, "execute unless score %s temp_variables = %s temp_variables run scoreboard players set %s temp_variables 1", res1.result_identifier, res2.result_identifier, temp_var_name);

        lines.push_back(clear_temp_var(temp_var_name));
        lines.push_back(temp);

        return { lines, temp_var_name };
    }

    if (expr->is<VariableReference>()) {
        VariableReference* var_ref = expr->as<VariableReference>();
        const char* temp_var_name = create_temporary_variable_identifier();
        char* temp = (char*)malloc(512);
        ErrorReporter::assert_or_internal_error(temp, expr->token(), "Failed to malloc");
        assert(temp);
        sprintf(temp, "scoreboard players operation %s temp_variables = %s dpl_variables", temp_var_name, var_ref->identifier());

        return { { temp }, temp_var_name };
    }

    if (expr->is<FunctionCall>()) {
        std::vector<const char*> call_lines = generate_function_call(expr->as<FunctionCall>());
        std::vector<const char*> lines{};
        extend_vector<const char*>(&lines, &call_lines);

        const char* temp_var_name = create_temporary_variable_identifier();
        char* temp = (char*)malloc(512);
        ErrorReporter::assert_or_internal_error(temp, expr->token(), "Failed to malloc");
        assert(temp);
        sprintf(temp, "scoreboard players operation %s temp_variables = __yield__ dpl_variables", temp_var_name);

        lines.push_back(temp);

        return { lines, temp_var_name };
    }

    // NOTE: Add missing ones here

    if (expr->is<Expression>()) {
        Expression* expr_expr = expr->as<Expression>();
        if (expr_expr->children().size() != 1)
        {
            ErrorReporter::report_error(expr_expr->token(), "This ASTNode has too many children, maybe it's not implemented yet?");
        }
        EvaluationResult res = evaluate_math_expression(expr_expr->children()[0]->as<Expression>());
        return res;
    }
    ErrorReporter::report_error(expr->token(), "Cannot evaluate mathematical expression, encountered unexpected expression");
}

const char* CodeGenerator::create_temporary_variable_identifier()
{
    char* temp = (char*)malloc(8);
    ErrorReporter::assert_or_internal_error(temp, nullptr, "Failed to malloc");
    assert(temp);
    sprintf(temp, "TMP_%d", ++m_temporary_vars);
    return temp;
}

std::vector<FunctionInformation> CodeGenerator::prepare_functions(Program* program)
{
    std::vector<FunctionInformation> results{};
    for (size_t i = 0; i < program->children().size(); i++)
    {
        if (program->children()[i]->is<FunctionDeclaration>())
        {
            FunctionDeclaration* decl = program->children()[i]->as<FunctionDeclaration>();
            results.push_back({ decl->identifier(), decl->params() });
        }
        if (program->children()[i]->is<TemplateDeclaration>())
        {
            TemplateDeclaration* decl = program->children()[i]->as<TemplateDeclaration>();
            if (decl->children().size() != 1) {
                ErrorReporter::report_error(decl->token(), "TemplateDeclaration must have excatly one child");
            }
            if (!decl->children()[0]->is<RawStatement>()) {
                ErrorReporter::report_error(decl->token(), "The child of a TemplateDeclaration must be a RawStatement");
            }
            RawStatement* raw = decl->children()[0]->as<RawStatement>();
            results.push_back({ decl->identifier(), raw });
        }
    }
    return results;
}

std::vector<CodeGeneratorResult> CodeGenerator::prepare_standard_files()
{
    std::vector<CodeGeneratorResult> ret{};
    {
            std::vector<const char*> lines = {};
            lines.push_back("{");
            lines.push_back("\"values\":[");
            lines.push_back("\"dpl:init\"");
            if (function_info_from_identifier("start")) {
                lines.push_back(",\"dpl:start/invoke\"");
            }
            lines.push_back("]");
            lines.push_back("}");
            CodeGeneratorResult start_tag = { "data/minecraft/tags/functions/load.json", lines, false };
            ret.push_back(start_tag);
    }
    if (function_info_from_identifier("tick")) {
        std::vector<const char*> lines = {};
        lines.push_back("{");
        lines.push_back("\"values\":[");
        lines.push_back("\"dpl:tick/invoke\"");
        lines.push_back("]");
        lines.push_back("}");
        CodeGeneratorResult tick_tag = { "data/minecraft/tags/functions/tick.json", lines, false };
        ret.push_back(tick_tag);
    }
    {
        std::vector<const char*> lines = {};
        lines.push_back("{\"pack\":{");
        lines.push_back("\"pack_format\": 10,");
        lines.push_back("\"description\": \"Datapack generaterd by the DPL compiler created by Gameefan\"");
        lines.push_back("}}");
        CodeGeneratorResult pack_mcmeta_file = { "pack.mcmeta", lines, false };
        ret.push_back(pack_mcmeta_file);
    }
    return ret;
}

std::vector<CodeGeneratorResult> CodeGenerator::generate_functions(Program* program)
{
    std::vector<CodeGeneratorResult> results{};
    for (size_t i = 0; i < program->children().size(); i++)
    {
        if (program->children()[i]->is<FunctionDeclaration>())
        {
            std::vector<CodeGeneratorResult> decl = generate_function_declaration(program->children()[i]->as<FunctionDeclaration>());
            extend_vector<CodeGeneratorResult>(&results, &decl);
        }
    }
    return results;
}

std::vector<CodeGeneratorResult> CodeGenerator::generate_function_declaration(FunctionDeclaration* decl)
{
    char* path = (char*)malloc(256);
    assert(path);
    sprintf(path, "data/dpl/functions/%s/invoke.mcfunction", decl->identifier());
    std::vector<const char*> lines{};

    std::vector<CodeGeneratorResult> ret{};

    for (size_t i = 0; i < decl->children().size(); i++)
    {
        ASTNode* child = decl->children()[i];
        if (child->is<FunctionCall>()) {
            std::vector<const char*> call_lines = generate_function_call(child->as<FunctionCall>());
            extend_vector<const char*>(&lines, &call_lines);
            continue;
        }
        if (child->is<AssignmentExpression>()) {
            std::vector<const char*> expr_lines = generate_assignment_expression(child->as<AssignmentExpression>());
            extend_vector<const char*>(&lines, &expr_lines);
            continue;
        }
        if (child->is<IfStatement>()) {
            IfStatement* if_stat = child->as<IfStatement>();
            ErrorReporter::assert_or_internal_error(if_stat->children().size() > 0, if_stat->token(), "IfStatement must have at least one child (Validator failed)");
            assert(if_stat->children().size() > 0);
            Expression* check = if_stat->children()[0]->as<Expression>();
            EvaluationResult res = evaluate_math_expression(check);
            extend_vector<const char*>(&lines, &res.lines);

            std::vector<CodeGeneratorResult> codegen_results = generate_block(child, true);
            extend_vector<CodeGeneratorResult>(&ret, &codegen_results);

            char* line = (char*)malloc(128);
            assert(line);
            sprintf(line, "execute if score %s temp_variables matches 1 run function dpl:%s/%s", res.result_identifier, decl->identifier(), generate_name_for_block(if_stat));
            lines.push_back(line);
            continue;
        }
        if (child->is<UnlessStatement>()) {
            UnlessStatement* unless_stat = child->as<UnlessStatement>();
            ErrorReporter::assert_or_internal_error(unless_stat->children().size() > 0, unless_stat->token(), "UnlessStatement must have at least one child (Validator failed)");
            assert(unless_stat->children().size() > 0);
            Expression* check = unless_stat->children()[0]->as<Expression>();
            EvaluationResult res = evaluate_math_expression(check);
            extend_vector<const char*>(&lines, &res.lines);

            std::vector<CodeGeneratorResult> codegen_results = generate_block(child, true);
            extend_vector<CodeGeneratorResult>(&ret, &codegen_results);

            char* line = (char*)malloc(128);
            assert(line);
            sprintf(line, "execute unless score %s temp_variables matches 1 run function dpl:%s/%s", res.result_identifier, decl->identifier(), generate_name_for_block(unless_stat));
            lines.push_back(line);
            continue;
        }
        if (child->is<RawStatement>())
        {
            std::vector<const char*> raw_stat = generate_raw_statement(child->as<RawStatement>());
            extend_vector<const char*>(&lines, &raw_stat);
            continue;
        }
        if (child->is<YieldStatement>())
        {
            Expression* expr = child->children()[0]->as<Expression>();
            EvaluationResult res = evaluate_math_expression(expr);

            extend_vector<const char*>(&lines, &res.lines);

            char* line = (char*)malloc(128);
            ErrorReporter::assert_or_internal_error(line, child->token(), "Failed to malloc");
            assert(line);
            sprintf(line, "scoreboard players operation __yield__ dpl_variables = %s temp_variables", res.result_identifier);

            lines.push_back(line);
            continue;
        }
        ErrorReporter::report_error(child->token(), "I don't know how to generate code for this node");
    }
    ret.push_back({ path, lines });

    return ret;
}

std::vector<CodeGeneratorResult> CodeGenerator::generate_block(ASTNode* block_parent, bool skip_first)
{
    const char* block_name = generate_name_for_block(block_parent);
    char* path = (char*)malloc(256);
    assert(path);
    FunctionDeclaration* parent_function = nullptr;
    {
        ASTNode* target = block_parent;
        while (target && !target->is<FunctionDeclaration>()) {
            target = target->parent();
        }
        ErrorReporter::assert_or_internal_error(target, block_parent->token(), "Invalid argument passed to generate_block(), a block must have a FunctionDeclaration ancestor");
        assert(target);
        parent_function = target->as<FunctionDeclaration>();
    }

    sprintf(path, "data/dpl/functions/%s/%s.mcfunction", parent_function->identifier(), block_name);

    std::vector<const char*> lines{};
    std::vector<CodeGeneratorResult> ret{};

    for (size_t i = skip_first ? 1 : 0; i < block_parent->children().size(); i++)
    {
        ASTNode* child = block_parent->children()[i];
        assert(child);
        if (child->is<FunctionCall>()) {
            std::vector<const char*> call_lines = generate_function_call(child->as<FunctionCall>());
            extend_vector<const char*>(&lines, &call_lines);
            continue;
        }
        if (child->is<AssignmentExpression>()) {
            std::vector<const char*> expr_lines = generate_assignment_expression(child->as<AssignmentExpression>());
            extend_vector<const char*>(&lines, &expr_lines);
            continue;
        }
        if (child->is<IfStatement>()) {
            IfStatement* if_stat = child->as<IfStatement>();
            ErrorReporter::assert_or_internal_error(if_stat->children().size() > 0, if_stat->token(), "IfStatement must have at least one child (Validator failed)");
            assert(if_stat->children().size() > 0);
            Expression* check = if_stat->children()[0]->as<Expression>();
            EvaluationResult res = evaluate_math_expression(check);
            extend_vector<const char*>(&lines, &res.lines);

            std::vector<CodeGeneratorResult> codegen_results = generate_block(child, true);
            extend_vector<CodeGeneratorResult>(&ret, &codegen_results);

            char* line = (char*)malloc(128);
            ErrorReporter::assert_or_internal_error(line, child->token(), "Failed to malloc");
            assert(line);
            sprintf(line, "execute if score %s temp_variables matches 1 run function dpl:%s/%s", res.result_identifier, parent_function->identifier(), generate_name_for_block(if_stat));
            lines.push_back(line);
            continue;
        }
        if (child->is<UnlessStatement>()) {
            UnlessStatement* unless_stat = child->as<UnlessStatement>();
            ErrorReporter::assert_or_internal_error(unless_stat->children().size() > 0, unless_stat->token(), "UnlessStatement must have at least one child (Validator failed)");
            assert(unless_stat->children().size() > 0);
            Expression* check = unless_stat->children()[0]->as<Expression>();
            EvaluationResult res = evaluate_math_expression(check);
            extend_vector<const char*>(&lines, &res.lines);

            std::vector<CodeGeneratorResult> codegen_results = generate_block(child, true);
            extend_vector<CodeGeneratorResult>(&ret, &codegen_results);

            char* line = (char*)malloc(128);
            ErrorReporter::assert_or_internal_error(line, child->token(), "Failed to malloc");
            assert(line);
            sprintf(line, "execute unless score %s temp_variables matches 1 run function dpl:%s/%s", res.result_identifier, parent_function->identifier(), generate_name_for_block(unless_stat));
            lines.push_back(line);
            continue;
        }
        if (child->is<RawStatement>())
        {
            std::vector<const char*> raw_stat = generate_raw_statement(child->as<RawStatement>());
            extend_vector<const char*>(&lines, &raw_stat);
            continue;
        }
        if (child->is<YieldStatement>())
        {
            Expression* expr = child->children()[0]->as<Expression>();
            EvaluationResult res = evaluate_math_expression(expr);

            extend_vector<const char*>(&lines, &res.lines);

            char* line = (char*)malloc(128);
            ErrorReporter::assert_or_internal_error(line, child->token(), "Failed to malloc");
            assert(line);
            sprintf(line, "scoreboard players operation __yield__ dpl_variables = %s temp_variables", res.result_identifier);

            lines.push_back(line);
            continue;
        }
        ErrorReporter::report_error(child->token(), "I don't know how to generate code for this node");
    }

    ret.push_back({ path, lines });

    return ret;
}

std::vector<const char*> CodeGenerator::generate_function_call(FunctionCall* call)
{
    std::vector<const char*> lines{};

    if (strcmp(call->callee(), "print") == 0)
    {
        return NativeFunctions::print(call);
    }

    if (strcmp(call->callee(), "score") == 0)
    {
        return NativeFunctions::score(call);
    }

    if (strcmp(call->callee(), "static_loop") == 0)
    {
        ErrorReporter::assert_or_error(call->params().size() >= 2, call->token(), "Not enough parameters");
        ErrorReporter::assert_or_error(call->params()[0].literal->is<TextLiteral>(), call->token(), "First parameter must be a text literal");
        FunctionInformation* info = function_info_from_identifier(call->params()[0].literal->as<TextLiteral>()->value());
        ErrorReporter::assert_or_error(info, call->token(), "The target function doesn't exist");
        assert(info);
        return NativeFunctions::static_loop(call, info->params);
    }

    FunctionInformation* info = function_info_from_identifier(call->callee());

    if (!info)
    {
        ErrorReporter::report_error(call->token(), "Function doesn't exist");
    }

    std::vector<FunctionCall::Param> func_params = call->params();

    if (info->is_template) {
        RawStatement* raw = info->template_code;
        raw->clear_params();
        for (FunctionCall::Param param : func_params) {
            raw->add_param({ param.identifier, param.literal });
        }
        return generate_raw_statement(raw);
    }

    ErrorReporter::assert_or_error(func_params.size() == info->params.size(), call->token(), "Invalid amount of call arguments");
    assert(func_params.size() == info->params.size());

    for (size_t i = 0; i < info->params.size(); i++)
    {
        const char* param = info->params[i];
        assert(param);
        FunctionCall::Param value = func_params[i];
        if (value.identifier) {
            // variable reference
            char* line = (char*)malloc(64);
            ErrorReporter::assert_or_internal_error(line, call->token(), "Failed to malloc");
            assert(line);
            sprintf(line, "scoreboard players operation %s dpl_variables = %s dpl_variables", param, value.identifier);
            lines.push_back(line);
            continue;
        }
        else if (value.literal->is<NumericLiteral>()) {
            // numeric literal
            char* line = (char*)malloc(64);
            ErrorReporter::assert_or_internal_error(line, call->token(), "Failed to malloc");
            assert(line);
            sprintf(line, "scoreboard players set %s dpl_variables %d", param, value.literal->as<NumericLiteral>()->value());
            lines.push_back(line);
            continue;
        }
        else {
            ErrorReporter::report_error(call->token(), "Invalid function call parameter");
        }
    }

    char* invoke_cmd = (char*)malloc(128);
    ErrorReporter::assert_or_internal_error(invoke_cmd, call->token(), "Failed to malloc");
    assert(invoke_cmd);
    sprintf(invoke_cmd, "function dpl:%s/invoke", call->callee());
    lines.push_back(invoke_cmd);

    return lines;
}

FunctionInformation* CodeGenerator::function_info_from_identifier(const char* identifier)
{
    for (size_t i = 0; i < m_functions.size(); i++)
    {
        if (strcmp(m_functions[i].identifier, identifier) == 0) {
            return &m_functions[i];
        }
    }
    return nullptr;
}

std::vector<const char*> CodeGenerator::generate_assignment_expression(AssignmentExpression* expr)
{
    char* line = (char*)malloc(128);
    ErrorReporter::assert_or_internal_error(line, expr->token(), "Failed to malloc");
    assert(line);

    EvaluationResult res = evaluate_math_expression(expr->children()[0]->as<Expression>());
    std::vector<const char*> lines{};
    sprintf(line, "scoreboard players operation %s dpl_variables = %s temp_variables", expr->identifier(), res.result_identifier);

    extend_vector<const char*>(&lines, &res.lines);
    lines.push_back(line);

    return lines;
}

const char* CodeGenerator::generate_name_for_block(ASTNode* block_parent)
{
    char* temp = (char*)malloc(64);
    ErrorReporter::assert_or_internal_error(temp, block_parent->token(), "Failed to malloc");
    assert(temp);
    sprintf(temp, "BLOCK_%s_%p", block_parent->node_name(), block_parent);
    for (char* p = temp; *p; ++p) *p = tolower(*p);
    return temp;
}

static const char* trim_line(char* str)
{
    char* end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0)
        return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = 0;
    return str;
}

std::vector<const char*> CodeGenerator::generate_raw_statement(RawStatement* raw)
{
    const char* raw_data = strdup(raw->value());

    std::vector<const char*> lines{};
    size_t current_param = 0;
    {
        char* context = nullptr;
        char* tmp = (char*)raw_data;
        char* token = strtok_s(tmp, "\n", &context);
        while (token)
        {
            const char* trimmed = trim_line(strdup(token));
            char text[512]{ 0 };
            const char* ptr = trimmed - 1;
            size_t index = 0;
            while (*(++ptr)) {
                if (*ptr == '%') {
                    if (*(ptr + 1) == '%') {
                        ptr++;
                    }
                    size_t used_param = -1;
                    if (isdigit(*(ptr + 1))) {
                        char nval[512]{ 0 };
                        char* nval_ptr = nval;
                        while (*(++ptr) != '%') {
                            *nval_ptr++ = *ptr;
                        }
                        used_param = atoi(nval) - (size_t)1;
                        ErrorReporter::assert_or_error(used_param < raw->params().size(), raw->token(), "Parameter index out of range");
                        assert(used_param < raw->params().size());
                    }
                    else {
                        ErrorReporter::assert_or_error(raw->params().size() > current_param, raw->token(), "Not enough format parameters passed to raw statement");
                        assert(raw->params().size() > current_param);
                        used_param = current_param++;
                    }
                    RawStatement::Param param = raw->params()[used_param];
                    if (param.identifier) {
                        ErrorReporter::report_error(raw->token(), "Raw statement arguments cannot include variable identifiers");
                    }
                    if (param.literal->is<TextLiteral>()) {
                        index += sprintf(text + index, "%s", param.literal->as<TextLiteral>()->value());
                        continue;
                    } else if (param.literal->is<NumericLiteral>()) {
                        index += sprintf(text + index, "%d", param.literal->as<NumericLiteral>()->value());
                        continue;
                    } else if (param.literal->is<SelectorLiteral>()) {
                        index += sprintf(text + index, "%s", param.literal->as<SelectorLiteral>()->value());
                        continue;
                    }
                    else {
                        ErrorReporter::report_error(raw->token(), "Invalid token type");
                    }
                    continue;
                }
                text[index++] = *ptr;
                ErrorReporter::assert_or_internal_error(index < 512, raw->token(), "Arbitrary limity passed: A line cannot be longer than 512 bytes");
                assert(index < 512);
            }
            lines.push_back(strdup(text));
            token = strtok_s(nullptr, "\n", &context);
        }
    }
    return lines;
}

void CodeGenerator::prepare_notice(std::vector<CodeGeneratorResult>& files)
{
    for (size_t i = 0; i < files.size(); i++)
    {
        CodeGeneratorResult* file = &files[i];
        if (file->contains_code)
        {
            std::vector<const char*> lines{};
            lines.push_back("#");
            lines.push_back("# This file has been generated by the DPL compiler");
            lines.push_back("# created by Gameefan.");
            lines.push_back("#");
            lines.push_back("");
            file->lines.insert(file->lines.begin(), lines.begin(), lines.end());
        }
    }
}

const char* CodeGenerator::clear_temp_var(const char* var)
{
    char* temp = (char*)malloc(128);
    ErrorReporter::assert_or_internal_error(temp, nullptr, "Failed to malloc");
    assert(temp);
    sprintf(temp, "scoreboard players reset %s temp_variables", var);
    return temp;
}
