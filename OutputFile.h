#pragma once
#include "Concat.h"
#include "File.h"
struct Job;

struct OutputFile : public Concat, public File
{
    bool flush(bool lastOne, const function<bool(Job*)>& execAsync = nullptr);
    void save(ConcatBucket* bucket, uint32_t count, const function<bool(Job*)>& execAsync = nullptr);
    bool openWrite();

    HANDLE               winHandle = INVALID_HANDLE_VALUE;
    vector<LPOVERLAPPED> overlappeds;
    int                  seekSave = 0;
};