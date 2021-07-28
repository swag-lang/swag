#pragma once
#include "Concat.h"
#include "File.h"
struct Job;

struct OutputFile : public Concat
{
    bool flush(bool lastOne);
    bool save(void* buffer, uint32_t count);
    bool openWrite();
    void close();

    Utf8                 name;
    string               path;
    vector<LPOVERLAPPED> overlappeds;
    HANDLE               winHandle = INVALID_HANDLE_VALUE;
    int                  seekSave  = 0;
};