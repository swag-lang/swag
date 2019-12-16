#pragma once
#include "Concat.h"
#include "File.h"
struct SavingThreadRequest;

struct OutputFile : public Concat, public File
{
    void flushBucket(ConcatBucket* bucket, bool flush, bool last) override;
    bool flush(bool lastOne);
    void notifySave(bool last);

    mutex              mutexNotify;
    condition_variable condVar;
    ConcatBucket*      lastFlushedBucket = nullptr;
    bool               done              = false;
    bool               lastOne           = false;
};