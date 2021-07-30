#pragma once
#include "Utf8.h"

bool openFile(FILE** fileHandle, const char* path, const char* mode);
void closeFile(FILE** fileHandle);

struct OutputFile
{
    virtual bool openWrite(const string& path)      = 0;
    virtual bool save(void* buffer, uint32_t count) = 0;
    virtual void close()                            = 0;
};
