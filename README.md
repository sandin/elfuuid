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

​                  

**Usage:**

```
> elfuuid libmain.so --type=breakpad
4BA667FEBEC3B9103B84B8FCD0054F900

> elfuuid libmain.so --type=bugly
c3be10b93b84b8fcd0054f901a618692
```

​                

**API:**

```
/**
 * Read Build ID from ELF file
 *
 * @param elf_filename elf filepath
 * @param uuid_type 0:breakpad 1:bugly
 * @return build_id
 */
std::string FindElfBuildID(const char *elf_filename, uint8_t uuid_type);
```

