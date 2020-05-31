#include "stdafx.h"
#pragma comment(lib,"ws2_32.lib")

#ifdef _WIN32
#include <winsock.h>
#elif __ANDROID__ || __linux__
#include <arpa/inet.h>
#endif //_WIN32

#include <fstream>
#include <algorithm>
#include <vector>
#include "elfutils.h"
#include <string.h>

#ifdef _WIN32
#ifndef MIN 
#define MIN min
#endif
#elif  __ANDROID__ || __linux__
#ifndef MIN 
#define MIN std::min
#endif
#endif // _WIN32



#ifndef NT_GNU_BUILD_ID
#define NT_GNU_BUILD_ID 3
#endif

#define NOTE_PADDING(a) ((a + 3) & ~3)

typedef struct {
	uint32_t data1;
	uint16_t data2;
	uint16_t data3;
	uint8_t  data4[8];
} MDGUID;  /* GUID */

// Used in a few places for backwards-compatibility.
const size_t kMDGUIDSize = sizeof(MDGUID);

#if _WIN32

#endif

// These three functions are not ever called in an unsafe context, so it's OK
// to allocate memory and use libc.
static std::string bytes_to_hex_string(const uint8_t* bytes, size_t count, bool uppercase = true) {
	std::string result;
	for (unsigned int idx = 0; idx < count; ++idx) {
		char buf[3];
		snprintf(buf, sizeof(buf), uppercase ? "%02X" : "%02x", bytes[idx]);
		result.append(buf);
	}
	return result;
}

static std::string BuglyBuildIdNoteIdentifier(const void *section_start, size_t section_size)
{
	int build_id_length = 16; 
	//printf("bugly build id, section_size=%d, build_id_length=%d\n", section_size, build_id_length);
	if (section_size > build_id_length) {
		const void* section_end = reinterpret_cast<const char*>(section_start) + section_size;
		const void* string_start = reinterpret_cast<const char*>(section_start) + (section_size - build_id_length);
		//printf("bugly build id, section_start=%x, section_end=%x, string_start=%x\n", section_start, section_end, string_start);
		return bytes_to_hex_string((const uint8_t*)string_start, build_id_length, false/* lowercase */);
	}
	return bytes_to_hex_string((const uint8_t*)section_start, section_size, false/* lowercase */); 
}

static std::string ElfClassBuildIDNoteIdentifier(const void *section_start, size_t section_size)
{
	std::vector<uint8_t> identifier;

	const void* section_end = reinterpret_cast<const char*>(section_start) + section_size;
	const Elf32_Nhdr* note_header = reinterpret_cast<const Elf32_Nhdr*>(section_start);
	while (reinterpret_cast<const void *>(note_header) < section_end) {
		if (note_header->n_type == NT_GNU_BUILD_ID)
			break;
		note_header = reinterpret_cast<const Elf32_Nhdr*>(
			reinterpret_cast<const char*>(note_header) + sizeof(Elf32_Nhdr) +
			NOTE_PADDING(note_header->n_namesz) +
			NOTE_PADDING(note_header->n_descsz));
	}
	if (reinterpret_cast<const void *>(note_header) >= section_end ||
		note_header->n_descsz == 0) {
		return ""; // TODO
	}

	const uint8_t* build_id = reinterpret_cast<const uint8_t*>(note_header) +
		sizeof(Elf32_Nhdr) + NOTE_PADDING(note_header->n_namesz);

	identifier.insert(identifier.end(),
		build_id,
		build_id + note_header->n_descsz);

	uint8_t identifier_swapped[kMDGUIDSize] = { 0 };
	// Endian-ness swap to match dump processor expectation.
	memcpy(identifier_swapped, &identifier[0], MIN(kMDGUIDSize, identifier.size()) );

	uint32_t* data1 = reinterpret_cast<uint32_t*>(identifier_swapped);
	*data1 = htonl(*data1);
	uint16_t* data2 = reinterpret_cast<uint16_t*>(identifier_swapped + 4);
	*data2 = htons(*data2);
	uint16_t* data3 = reinterpret_cast<uint16_t*>(identifier_swapped + 6);
	*data3 = htons(*data3);

	return bytes_to_hex_string(identifier_swapped, kMDGUIDSize);
}


std::string FindElfBuildID(const char* elf_filename, uint8_t uuid_type)
{
	std::string build_id = ""; // TODO
	void* note_section;
	size_t note_size;
	if (FindElfSection(elf_filename, ".note.gnu.build-id", (const void**)&note_section, &note_size))
	{
		if (uuid_type == 1)
		{
			// bugly style 
			build_id = BuglyBuildIdNoteIdentifier(note_section, note_size);
		}
		else 
		{
            // breakpad style (default)
			build_id = ElfClassBuildIDNoteIdentifier(note_section, note_size) + "0";
		}
	}
	if (note_section) 
	{
		free(note_section);
	}
	return build_id;
}

template<typename T_Elf_Ehdr, typename T_Elf_Shdr>
static bool FindElfClassSection(std::ifstream* f, const char* section_name, const void **section_start, size_t *section_size) 
{
	T_Elf_Ehdr ehdr;
	f->read((char*)&ehdr, sizeof(T_Elf_Ehdr));
	if (!IS_ELF(ehdr)) {
		printf("This is not a ELF file.\n");
		return false;
	}

	// .shstr (Section Names)
	T_Elf_Shdr section_names;
	uint64_t shstroff = ehdr.e_shoff + (sizeof(T_Elf_Shdr) * ehdr.e_shstrndx);
	f->seekg(shstroff, std::ios::beg);
	f->read((char*)&section_names, sizeof(T_Elf_Shdr));
	//printf("[Section] index=%d, name=.shstr, section_header_offset=%d, file_offset=%d, file_size=%d\n", ehdr.e_shstrndx, shstroff, section_names.sh_offset, section_names.sh_size);

	unsigned char* names = (unsigned char*)malloc(section_names.sh_size);
	f->seekg(section_names.sh_offset, std::ios::beg);
	f->read((char*)names, section_names.sh_size);

	// loop sections
	//printf("Section header offset=%d, section count=%d\n", (unsigned int)ehdr.e_shoff, (unsigned int)ehdr.e_shnum);
	f->seekg(ehdr.e_shoff, std::ios::beg);
	uint32_t nsection = ehdr.e_shnum;
	T_Elf_Shdr section;
	bool found = false;
	for (int i = 0; i < nsection; ++i) {
		f->read((char*)&section, sizeof(T_Elf_Shdr));
		const char* section_name = (char*)names + section.sh_name;
		//printf("[Section] index=%-3d name=%-20s type=%-2d sh_offset=%-8d sh_size=%-8d\n", i, section_name, section.sh_type, section.sh_offset, section.sh_size);
		if (strcmp(section_name, ".note.gnu.build-id") == 0) {
			unsigned char* buffer = (unsigned char*)malloc(section.sh_size);
			f->seekg(section.sh_offset, std::ios::beg);
			f->read((char*)buffer, section.sh_size);
			*section_start = buffer;
			*section_size = section.sh_size;
			found = true;
			break;
		}
	}

	free(names);
	return found;
}

bool FindElfSection(const char *elf_filename, const char* section_name, const void **section_start, size_t *section_size) 
{
	bool result = false;

	*section_start = NULL;
	*section_size = 0;

	std::ifstream f(elf_filename, std::ios::binary);

	unsigned char e_ident[EI_NIDENT];
	f.read((char*)&e_ident, EI_NIDENT);
	f.clear();
	f.seekg(0);
	if (e_ident[EI_CLASS] == ELFCLASS32)
	{
		result = FindElfClassSection<Elf32_Ehdr, Elf32_Shdr>(&f, section_name, section_start, section_size);
	}
	else
	{
		result = FindElfClassSection<Elf64_Ehdr, Elf64_Shdr>(&f, section_name, section_start, section_size);
	}
	f.close();
	return result;
}
