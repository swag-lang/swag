#include "pch.h"
#include "OutputFile.h"
#include "ThreadManager.h"
#include "SavingThread.h"

void OutputFile::flushBucket(ConcatBucket* bucket)
{
    lastFlushedBucket = bucket;
    auto req          = g_ThreadMgr.savingThread->newRequest();
    req->file         = this;
    req->buffer       = (char*) bucket->datas;
    req->bufferSize   = bucket->count;
    req->firstSave    = firstSave;
    firstSave         = false;
    lastRequest       = req;
    pendingRequests++;
    g_ThreadMgr.savingThread->addRequest(req);
}

bool OutputFile::flush()
{
    bool result = true;
    auto bucket = lastFlushedBucket ? lastFlushedBucket->nextBucket : firstBucket;

    while (bucket)
    {
        flushBucket(bucket);
        bucket = bucket->nextBucket;
    }

    while (true)
    {
        unique_lock lk(mutexNotify);
        if (!pendingRequests)
        {
            SWAG_ASSERT(reqToRelease.empty());
            break;
        }

        condVar.wait(lk);
        for (auto req : reqToRelease)
        {
            if (req->ioError)
                result = false;
            g_ThreadMgr.savingThread->releaseRequest(req);
        }

        reqToRelease.clear();
    }

    SWAG_ASSERT(!pendingRequests);
    if (openFile)
        fclose(openFile);
    return result;
}

void OutputFile::notifySave(SaveThreadRequest* req)
{
    unique_lock lk(mutexNotify);
    reqToRelease.push_back(req);
    pendingRequests--;
    condVar.notify_one();
}