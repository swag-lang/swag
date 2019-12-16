#pragma once
#include "Concat.h"
#include "File.h"
struct SaveRequest;

struct SaveRequest
{
    char* buffer     = nullptr;
    long  bufferSize = 0;
    bool  ioError    = false;
};

struct OutputFile : public Concat, public File
{
    void flushBucket(ConcatBucket* bucket) override;
    bool flush(bool lastOne);
    void save(SaveRequest* request);

    ConcatBucket* lastFlushedBucket = nullptr;
};