#pragma once
#include "Concat.h"
#include "File.h"
struct Job;

struct OutputFile : public Concat
{
    bool flush(bool lastOne, uint8_t pendingAffinity);
    bool save(void* buffer, uint32_t count, uint8_t pendingAffinity);
    bool openWrite();
    void close();

    Utf8                 name;
    string               path;
    HANDLE               winHandle = INVALID_HANDLE_VALUE;
    vector<LPOVERLAPPED> overlappeds;
    int                  seekSave = 0;
};