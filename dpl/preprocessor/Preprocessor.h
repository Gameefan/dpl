#pragma once
#include <vector>

class Preprocessor {
public:
	const char* run(const char* data);

private:
	struct PreprocessorParsingResult {
		const char* text_to_place = nullptr;
		bool is_if = false;
		bool is_endif = false;
		bool skip_to_endif = false;
	};
	PreprocessorParsingResult parse_preprocessor_instruction(const char* text);

	struct PreprocessorDefinition {
		const char* name;
		const char* value;
	};
	std::vector<PreprocessorDefinition> m_definitions{};
};