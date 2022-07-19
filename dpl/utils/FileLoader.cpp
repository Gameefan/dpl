#include "FileLoader.h"
#include <cstdio>
#include <malloc.h>
#include <cassert>
#include <sys/stat.h>
#include <cstdlib>

#ifdef _WIN32
#include <io.h>
#define F_OK 0
#define access _access
#endif

const char* FileLoader::load_file(const char* path)
{
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
