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


/**
 * Convert Build id into Breakpad UUID
 * 
 * @param build_id build id
 * @return breakpad uuid
 */
std::string ConvertBuildIdToBreakpadUUID(std::string build_id); 

/**
 * Convert Macho UUID into Breakpad UUID
 * 
 * @param build_id build id
 * @return breakpad uuid
 */
std::string ConvertMachoIdToBreakpadUUID(std::string build_id); 


#ifdef __cplusplus
} // extern "C"
#endif


#endif//_ELF_UTILS_WRAPPER_H_