#include <string.h>
#include <stdio.h>

#include "elfutils_wrap.h"
#include "elfutils.h"


char* CFindElfBuildID(const char *elf_filename, int uuid_type) {
    std::string result = FindElfBuildID(elf_filename, uuid_type);
    const char* build_id = result.c_str();
    char* build_id_str = (char*)malloc(strlen(build_id) + 1);
    strcpy(build_id_str, build_id);
    return build_id_str;
}
