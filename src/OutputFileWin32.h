#pragma once
#include "File.h"
struct Job;

struct OutputFileWin32 : public OutputFile
{
    bool openWrite(const string& path) override;
    bool save(void* buffer, uint32_t count) override;
    void close() override;

    string filePath;
    HANDLE winHandle = INVALID_HANDLE_VALUE;
    int    seekSave  = 0;
};