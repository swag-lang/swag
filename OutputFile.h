#pragma once
#include "Concat.h"
struct SaveThreadRequest;

struct OutputFile : public Concat
{
    string                     fileName;
    ConcatBucket*              lastFlushedBucket = nullptr;
    SaveThreadRequest*         lastRequest       = nullptr;
    FILE*                      openFile          = nullptr;
    mutex                      mutexNotify;
    condition_variable         condVar;
    vector<SaveThreadRequest*> reqToRelease;
    int                        pendingRequests = 0;
    bool                       firstSave       = true;

    void flushBucket(ConcatBucket* bucket) override;
    bool flush();
    void notifySave(SaveThreadRequest* req);
};