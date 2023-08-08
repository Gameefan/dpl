#include "Exporter.h"
#include <filesystem>
#include <cstdlib>
#include "../utils/ErrorReporter.h"

#ifdef _WIN32
#include <direct.h>
#define chdir _chdir
#else
#include <unistd.h>
#endif

void Exporter::run(std::vector<CodeGeneratorResult> code)
{
	Directory* root_dir = new Directory({ "dpl_output" });
	for (CodeGeneratorResult cgr : code)
	{
		char* tmp = strdup(cgr.filename);
		char* context = nullptr;
#ifdef _WIN32
		char* token = strtok_s(tmp, "/", &context);
#else
		char* token = strtok_r(tmp, "/", &context);
#endif
		Directory* target_dir = root_dir;
		while (token)
		{
			const char* name = token;
#ifdef _WIN32
			token = strtok_s(nullptr, "/", &context);
#else
			token = strtok_r(nullptr, "/", &context);
#endif
			if (!token) {
				// This is a file
				target_dir->files.push_back(new File({ name, cgr.filename, cgr.lines }));
			}
			else {
				// This is a dir
				target_dir = target_dir->ensure_directory(name);
			}
		}
	}
	create_directory(root_dir);
	create_files(code);
	std::filesystem::remove("dpl_output.zip");
#ifdef _WIN32
	system("7z a dpl_output.zip ./dpl_output/* >nul");
#else
	system("7z a dpl_output.zip ./dpl_output/* >/dev/null");
#endif
	std::filesystem::remove_all("dpl_output");
}

void Exporter::create_directory(Directory* dir)
{
	if (!std::filesystem::create_directory(dir->dirname))
	{
		ErrorReporter::report_error(nullptr, "Failed to create directory");
	}
	if (chdir(dir->dirname))
	{
		perror("Failed to enter directory");
		abort();
	}
	for (Directory* child_dir : dir->dirs) {
		create_directory(child_dir);
	}
	if (chdir(".."))
	{
		perror("Failed to exit directory");
		abort();
	}
}

void Exporter::create_files(std::vector<CodeGeneratorResult> files)
{
	if (chdir("dpl_output"))
	{
		perror("Failed to enter directory");
		abort();
	}
	for (CodeGeneratorResult cgr : files) {
		FILE* file = fopen(cgr.filename, "a");
		if (!file) {
			perror("Failed to open file");
			abort();
		}
		for (const char* line : cgr.lines) {
			fprintf(file, "%s\n", line);
		}
		fclose(file);
	}
	if (chdir(".."))
	{
		perror("Failed to exit directory");
		abort();
	}
}

Directory* Directory::ensure_directory(const char* name)
{
	for (size_t i = 0; i < dirs.size(); i++)
	{
		if (strcmp(dirs[i]->dirname, name) == 0)
		{
			return dirs[i];
		}
	}
	Directory* dir = new Directory({ name });
	dirs.push_back(dir);
	return dir;
}

void Directory::dump(int depth)
{
	for (size_t i = 0; i < depth; i++)
	{
		printf("\t");
	}
	printf("[%s]\n", dirname);
	for (Directory* dir : dirs)
	{
		dir->dump(depth + 1);
	}
	for (File* file : files)
	{
		for (size_t i = 0; i < depth + 1; i++)
		{
			printf("\t");
		}
		printf("%s\n", file->filename);
	}
}
