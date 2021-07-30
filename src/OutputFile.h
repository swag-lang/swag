#pragma once
#include "File.h"
struct Job;

struct OutputFile
{
    bool openWrite(const string& path);
    bool save(void* buffer, uint32_t count);
    void close();

    string filePath;
    HANDLE winHandle = INVALID_HANDLE_VALUE;
    int    seekSave  = 0;
};