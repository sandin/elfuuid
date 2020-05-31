package main

// #include <string.h>
// #include <stdio.h>
// #include <stdlib.h>
// #cgo CFLAGS: -I../src
// #cgo CXXFLAGS: -std=c++11
// #cgo LDFLAGS: -L${SRCDIR} -L./ -L../build/ -lelfutils
// #include "elfutils_wrap.h"
import "C"

import (
	"fmt"
	"unsafe"
)

func myassert(condition bool, msg string) {
	if !condition {
		fmt.Errorf("assert fail: %s", msg)
	}
}

func getBuildID(elfFileName string, uuidType int) string {
	buildID := C.CFindElfBuildID(C.CString(elfFileName), C.int(uuidType))
	defer C.free(unsafe.Pointer(buildID))
	return C.GoString(buildID)
}

func main() {
	buildID := getBuildID("libmain.so", 0)
	fmt.Printf("breakpad buildId=%s\n", buildID)
	myassert(buildID == "4BA667FEBEC3B9103B84B8FCD0054F900", "breakpad uuid")

	buildID = getBuildID("libmain.so", 1)
	fmt.Printf("bugly buildId=%s\n", buildID)
	myassert(buildID == "4c3be10b93b84b8fcd0054f901a618692", "bugly uuid")

	fmt.Printf("OK\n")
}
