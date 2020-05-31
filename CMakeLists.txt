cmake_minimum_required(VERSION 3.5)

project (elfuuid)

SET(SOURCE_FILES
    src/elfutils.cpp
    src/elfutils_wrap.cpp
    src/main.cpp
)

add_executable(elfuuid ${SOURCE_FILES})
add_library(elfutils SHARED ${SOURCE_FILES})