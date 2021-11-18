# elfuuid

Read build id from a elf file.

​         

**Build:**

```
mkdir build
cd build
cmake ..
make
```

**Test:**


```
cd test
go build
LD_LIBRARY_PATH=../build ./test
```
​                  

**Usage:**


Get Breakpad UUID:
```
> elfuuid --type=breakpad libmain.so
4BA667FEBEC3B9103B84B8FCD0054F900
```

Get Bugly UUID:
```
> elfuuid --type=bugly libmain.so
c3be10b93b84b8fcd0054f901a618692
```


Get Build ID:
```
> elfuuid --type=buildId libmain.so 
d8d07629fc319d0972c10b36619e08df5f68dd10
```

Convert Build ID to Breakpad UUID:
```
> elfuuid --type=buildId2breakpad d8d07629fc319d0972c10b36619e08df5f68dd10
2976D0D831FC099D72C10B36619E08DF0
```
​                

**API:**

C++:
```
#include "elfutils.h"


/**
 * Read Build ID from ELF file
 *
 * @param elf_filename elf filepath
 * @param uuid_type 0:breakpad 1:bugly
 * @return build_id
 */
std::string FindElfBuildID(const char *elf_filename, uint8_t uuid_type);
```

C:
```
#include "elfutils_wrap.h"


/**
 * Read Build ID from ELF file
 *
 * @param elf_filename elf filepath
 * @param uuid_type 0:breakpad 1:bugly
 * @return build_id
 */
char* CFindElfBuildID(const char *elf_filename, int uuid_type);
```
