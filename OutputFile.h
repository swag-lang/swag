#pragma once
#include "Concat.h"
struct SavingThreadRequest;

struct OutputFile : public Concat
{
    void flushBucket(ConcatBucket* bucket, bool lastOne) override;
    bool flush();
    void notifySave();

    string             fileName;
    mutex              mutexNotify;
    condition_variable condVar;
    ConcatBucket*      lastFlushedBucket = nullptr;
    FILE*              openFile          = nullptr;
    bool               firstSave         = true;
};