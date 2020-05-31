#ifndef _ELF_UTILS_WRAPPER_H_
#define _ELF_UTILS_WRAPPER_H_


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Read Build ID from ELF file
 *
 * @param elf_filename elf filepath
 * @param uuid_type 0:breakpad 1:bugly
 * @return build_id
 */
char* CFindElfBuildID(const char *elf_filename, int uuid_type);


#ifdef __cplusplus
} // extern "C"
#endif


#endif//_ELF_UTILS_WRAPPER_H_