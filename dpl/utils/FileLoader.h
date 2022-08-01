#pragma once
#include <vector>

class FileLoader {
public:
	static const char* load_file(const char* path);
private:
	inline static std::vector<const char*> s_included_files{};
};