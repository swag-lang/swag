#pragma once
#include "Concat.h"
struct SavingThreadRequest;

struct OutputFile : public Concat
{
    OutputFile()
    {
        bucketSize = 1024;
    }

    void flushBucket(ConcatBucket* bucket, bool flush, bool last) override;
    bool flush(bool lastOne);
    void notifySave(bool last);

    string             fileName;
    mutex              mutexNotify;
    condition_variable condVar;
    ConcatBucket*      lastFlushedBucket = nullptr;
    FILE*              fileHandle        = nullptr;
    bool               firstSave         = true;
    bool               done              = false;
    bool               lastOne           = false;
};