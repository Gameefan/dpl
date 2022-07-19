#pragma once
#include "../parser/AST.h"

static class NativeFunctions {
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
					output_ptr += sprintf(text + output_ptr, "{\"score\":{\"name\":\"%s\",\"objective\":\"dpl_variables\"},\"color\":\"white\"}", param.identifier);
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
		sprintf(text, "scoreboard players operation __yield__ dpl_variables = %s %s", selector, objective);
		return { strdup(text) };
	}
};