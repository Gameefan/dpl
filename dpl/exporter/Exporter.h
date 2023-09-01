#pragma once
#include "../codegen/CodeGenerator.h"
#include <vector>

struct File {
	const char* filename;
	const char* full_path;
	std::vector<const char*> lines{};
};

struct Directory {
	const char* dirname;
	std::vector<File*> files{};
	std::vector<Directory*> dirs{};
	Directory* ensure_directory(const char* name);
	void dump(int depth = 0);
};

class Exporter {
public:
	Exporter(const char* package_name)
		: m_package_name(package_name)
	{}
	void run(std::vector<CodeGeneratorResult> code);
private:
	void create_directory(Directory* dir);
	void create_files(std::vector<CodeGeneratorResult> files);
	const char* m_package_name = "dpl";
};