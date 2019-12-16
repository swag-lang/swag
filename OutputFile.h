#pragma once
#include "Concat.h"
#include "File.h"
struct SaveRequest;

struct OutputFile : public Concat, public File
{
    void flushBucket(ConcatBucket* bucket) override;
    bool flush(bool lastOne);

    ConcatBucket* lastFlushedBucket = nullptr;
    bool          lastOne           = false;
};