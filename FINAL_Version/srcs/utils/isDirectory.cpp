#include <Utils.hpp>
#include <sys/stat.h>

bool isDirectory(const char* path) {
    struct stat info;
    if (stat(path, &info) != 0) {
        return false;
    }

    return S_ISDIR(info.st_mode);
}
