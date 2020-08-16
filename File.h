#pragma once
#include "Utf8.h"

struct File
{
    bool openRead();
    void close();

    static void openFile(FILE** fileHandle, const char* path, const char* mode);
    static void closeFile(FILE** fileHandle);

    Utf8   name;
    string path;
    FILE*  fileHandle = nullptr;
    bool   openedOnce = false;
};
