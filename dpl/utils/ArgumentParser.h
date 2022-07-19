#pragma once

class ArgumentParser {
public:
	bool run(int argc, char** argv);
	bool dump_tokens() { return m_dump_tokens; }
	bool dump_ast() { return m_dump_ast; }
	bool dump_ast_tokens() { return m_dump_ast_tokens; }
	bool optimize_ast() { return m_optimize_ast; }
	bool dump_optimized_ast() { return m_dump_optimized_ast; }
	const char* file() { return m_file; }
private:
	bool m_dump_tokens = false;
	bool m_dump_ast = false;
	bool m_dump_ast_tokens = false;
	bool m_optimize_ast = false;
	bool m_dump_optimized_ast = false;
	const char* m_file = "test.dpl";
	void show_help(const char* progname);
};