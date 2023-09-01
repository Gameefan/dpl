#pragma once
#include "../parser/AST.h"
#include "../utils/ErrorReporter.h"
#include <cstring>

class NativeFunctions {
public:
	static std::vector<const char*> print(FunctionCall* call)
	{
		std::vector<const char*> segments;
		segments.push_back("\"\"");
		char text[1024]{ 0 };
		assert(call->params().size() >= 1);
		const char* format = call->params()[0].literal->as<TextLiteral>()->value();
		const char* ptr = format;
		assert(format);
		size_t output_ptr = 0;
		output_ptr += sprintf(text, "tellraw @a [\"\",{\"text\":\"");
		size_t current_arg = 0;
		do
		{
			assert(output_ptr < 1023);
			char c = *ptr;
			if (c == '%') {
				output_ptr += sprintf(text + output_ptr, "\",\"color\":\"gray\"},");
				assert(call->params().size() >= ++current_arg);
				FunctionCall::Param param = call->params()[current_arg];
				if (param.identifier) {
					output_ptr += sprintf(text + output_ptr, "{\"score\":{\"name\":\"%s\",\"objective\":\"%s_dpl_vars\"},\"color\":\"white\"}", param.identifier, call->package_name());
				}
				else if (param.literal->is<TextLiteral>()) {
					output_ptr += sprintf(text + output_ptr, "{\"text\":\"%s\",\"color\":\"white\"}", param.literal->as<TextLiteral>()->value());
				}
				else if (param.literal->is<NumericLiteral>()) {
					output_ptr += sprintf(text + output_ptr, "{\"text\":\"%d\",\"color\":\"white\"}", param.literal->as<NumericLiteral>()->value());
				}
				else if (param.literal->is<SelectorLiteral>()) {
					output_ptr += sprintf(text + output_ptr, "{\"selector\":\"%s\",\"color\":\"white\"}", param.literal->as<SelectorLiteral>()->value());
				}
				else {
					printf("Error: Invalid literal type\n");
					abort();
				}
				output_ptr += sprintf(text + output_ptr, ",{\"text\":\"");
				continue;
			}
			text[output_ptr++] = c;
			text[output_ptr] = 0;
		} while (*(++ptr) != 0);
		output_ptr += sprintf(text + output_ptr, "\",\"color\":\"gray\"}]");
		return { strdup(text) };
	}
	static std::vector<const char*> score(FunctionCall* call)
	{
		assert(call->params().size() == 2);
		const char* selector = call->params()[0].literal->as<SelectorLiteral>()->value();
		const char* objective = call->params()[1].literal->as<TextLiteral>()->value();
		char text[512];
		sprintf(text, "scoreboard players operation __yield__ %s_dpl_vars = %s %s", call->package_name(), selector, objective);
		return { strdup(text) };
	}
	static std::vector<const char*> static_loop(FunctionCall* call, std::vector<const char*> params)
	{
		const char* target_func = call->params()[0].literal->as<TextLiteral>()->value();
		ErrorReporter::assert_or_error(strcmp(target_func, "static_loop") != 0, call->token(), "You cannot use the static_loop function as the first argument of the static_loop function");

		ErrorReporter::assert_or_error(call->params()[1].literal->is<NumericLiteral>(), call->token(), "Second argument must be a numeric literal");
		int target_loop_count = call->params()[1].literal->as<NumericLiteral>()->value();

		ErrorReporter::assert_or_error(params.size() >= 1, call->token(), "The callee must have at least one input parameter");

		std::vector<const char*> lines{};

		for (int loop_index = 0; loop_index < target_loop_count; loop_index++)
		{
			{
				char* line = (char*)malloc(64);
				ErrorReporter::assert_or_internal_error(line, call->token(), "Failed to malloc");
				assert(line);
				sprintf(line, "scoreboard players set %s %s_dpl_vars %d", params[0], call->package_name(), loop_index);
				lines.push_back(line);
			}
			for (size_t i = 2; i < call->params().size(); i++)
			{
				const char* param = params[i-1];
				assert(param);
				FunctionCall::Param value = call->params()[i];
				if (value.identifier) {
					// variable reference
					char* line = (char*)malloc(64);
					ErrorReporter::assert_or_internal_error(line, call->token(), "Failed to malloc");
					assert(line);
					// FIXME: The last package name should be the local package
					sprintf(line, "scoreboard players operation %s %s_dpl_vars = %s %s_dpl_vars", param, call->package_name(), value.identifier, call->package_name());
					lines.push_back(line);
					continue;
				}
				else if (value.literal->is<NumericLiteral>()) {
					// numeric literal
					char* line = (char*)malloc(64);
					ErrorReporter::assert_or_internal_error(line, call->token(), "Failed to malloc");
					assert(line);
					sprintf(line, "scoreboard players set %s %s %d", param, call->package_name(), value.literal->as<NumericLiteral>()->value());
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
			// FIXME: Strip the package name from target_fun
			sprintf(invoke_cmd, "function %s:%s/invoke", call->package_name(), target_func);
			lines.push_back(invoke_cmd);
		}

		return lines;
	}
};
