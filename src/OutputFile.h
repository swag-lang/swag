#pragma once
#include "Concat.h"
#include "File.h"
struct Job;

struct OutputFile : public Concat, public File
{
    bool flush(bool lastOne, uint8_t pendingAffinity);
    bool save(void* buffer, uint32_t count, uint8_t pendingAffinity);
    bool openWrite();
    void close();

    HANDLE               winHandle = INVALID_HANDLE_VALUE;
    vector<LPOVERLAPPED> overlappeds;
    int                  seekSave = 0;
};