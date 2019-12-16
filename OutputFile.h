#pragma once
#include "Concat.h"
#include "File.h"
struct SavingThreadRequest;

struct OutputFile : public Concat, public File
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
    bool               firstSave         = true;
    bool               done              = false;
    bool               lastOne           = false;
};