#include "FileLoader.h"
#include <cstdio>
#include <malloc.h>
#include <cassert>
#include <sys/stat.h>
#include <cstdlib>
#include <cstring>

#ifdef _WIN32
#include <io.h>
#define F_OK 0
#define access _access
#else
#include <unistd.h>
#endif

const char* FileLoader::load_file(const char* path)
{
    for (const char* file : s_included_files)
    {
        if (strcmp(file, path) == 0)
        {
            // Don't include the file again to avoid name conflicts
            return "";
        }
    }
    s_included_files.push_back(strdup(path));
    if (access(path, F_OK) != 0)
    {
        printf("Error: Unable to load file %s, aborting compilation", path);
        abort();
    }
    char* data = nullptr;
    auto* file = fopen(path, "rb");
    if (!file) {
        perror("fopen");
        return nullptr;
    }
    fseek(file, 0, SEEK_END);
    size_t length = ftell(file);
    fseek(file, 0, SEEK_SET);
    data = (char*)malloc(length + 1);
    if (!data) {
        perror("malloc");
        return nullptr;
    }
    char* temp = data;
    while (fread((void*)temp, 1, 1, file) > 0) {
        temp++;
        fflush(file);
    }
    assert(feof(file));
    assert(!ferror(file));

    data[length] = 0;

    printf("%s is loaded at %p\n", path, data);
    printf("Read %llu bytes from %s\n", length, path);
    return data;
}
