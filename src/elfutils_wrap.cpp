#include <string.h>
#include <stdio.h>
#include <string>

#include "elfutils_wrap.h"
#include "elfutils.h"


char* CFindElfBuildID(const char *elf_filename, int uuid_type) {
    std::string result = FindElfBuildID(elf_filename, uuid_type);
    const char* build_id = result.c_str();
    char* build_id_str = (char*)malloc(strlen(build_id) + 1);
    strcpy(build_id_str, build_id);
    return build_id_str;
}


std::string ConvertBuildIdToBreakpadUUID(std::string build_id) {
    std::vector<uint8_t> bytes = hex_string_to_bytes(build_id);
    return ConvertIdentifierToUUIDString(bytes);
}

static void ConvertIdentifierToString(const unsigned char identifier[16],
                                       char *buffer, int buffer_length) {
    int buffer_idx = 0;
    for (int idx = 0; (buffer_idx < buffer_length) && (idx < 16); ++idx) {
        int hi = (identifier[idx] >> 4) & 0x0F;
        int lo = (identifier[idx]) & 0x0F;

        if (idx == 4 || idx == 6 || idx == 8 || idx == 10)
        buffer[buffer_idx++] = '-';

        buffer[buffer_idx++] =
            static_cast<char>((hi >= 10) ? ('A' + hi - 10) : ('0' + hi));
        buffer[buffer_idx++] =
            static_cast<char>((lo >= 10) ? ('A' + lo - 10) : ('0' + lo));
    }

    // NULL terminate
    buffer[(buffer_idx < buffer_length) ? buffer_idx : buffer_idx - 1] = 0;
}

std::string ConvertMachoIdToBreakpadUUID(std::string macho_uuid) {
    std::vector<uint8_t> identifier = hex_string_to_bytes(macho_uuid);

    unsigned char identifier_bytes[16];
    for (int i = 0; i < sizeof(identifier_bytes); i++) {
        if (i < identifier.size()) {
            identifier_bytes[i] = identifier[i];
        } else {
            identifier_bytes[i] = '\0';
        }
    }

    char identifier_string[40];
    size_t buffer_length = sizeof(identifier_string);
    ConvertIdentifierToString(identifier_bytes, identifier_string, sizeof(identifier_string));

    std::string compacted(identifier_string);
    for(size_t i = compacted.find('-'); i != std::string::npos; i = compacted.find('-', i)) {
        compacted.erase(i, 1);
    }

    return compacted + "0"; // std::string
}
