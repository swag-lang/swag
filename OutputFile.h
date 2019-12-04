#pragma once
#include "Concat.h"
struct SavingThreadRequest;

struct OutputFile : public Concat
{
    void flushBucket(ConcatBucket* bucket, bool lastOne = false) override;
	void flushBucket1(ConcatBucket* bucket, bool lastOne = false);
    bool flush();
    void notifySave();

    string             fileName;
    mutex              mutexNotify;
    condition_variable condVar;
    ConcatBucket*      lastFlushedBucket = nullptr;
    FILE*              openFile          = nullptr;
    bool               firstSave         = true;
    bool               done              = false;
};