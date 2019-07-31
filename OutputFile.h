#pragma once
#include "Concat.h"
struct SaveThreadRequest;

struct OutputFile : public Concat
{
    string                     fileName;
    ConcatBucket*              lastFlushedBucket = nullptr;
    SaveThreadRequest*         lastRequest       = nullptr;
    mutex                      mutexNotify;
    condition_variable         condVar;
    vector<SaveThreadRequest*> reqToRelease;
    int                        pendingRequests = 0;

    void flushBucket(ConcatBucket* bucket) override;
    void flush();
    void notifySave(SaveThreadRequest* req);
};