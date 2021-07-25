#pragma once
#include "Utf8.h"

struct File
{
    static void openFile(FILE** fileHandle, const char* path, const char* mode);
    static void closeFile(FILE** fileHandle);
};
