#ifndef _ELF_UTILS_H_
#define _ELF_UTILS_H_

#ifdef _WIN32
#include "elf.h"

typedef struct {
	Elf32_Word n_namesz;
	Elf32_Word n_descsz;
	Elf32_Word n_type;
} Elf32_Nhdr;
#elif __ANDROID__ || __linux__
#include <elf.h>
#endif

#ifndef IS_ELF
/* e_ident */
#define IS_ELF(ehdr) ((ehdr).e_ident[EI_MAG0] == ELFMAG0 && \
                      (ehdr).e_ident[EI_MAG1] == ELFMAG1 && \
                      (ehdr).e_ident[EI_MAG2] == ELFMAG2 && \
                      (ehdr).e_ident[EI_MAG3] == ELFMAG3)
#endif

#include <string>

/**
 * Read Build ID from ELF file
 *
 * @param elf_filename elf filepath
 * @param uuid_type 0:breakpad 1:bugly
 * @return build_id
 */
std::string FindElfBuildID(const char *elf_filename, uint8_t uuid_type);

bool FindElfSection(const char *elf_filename, const char *section_name, const void **section_start, size_t *section_size);

#endif /* _ELF_UTILS_H_ */
