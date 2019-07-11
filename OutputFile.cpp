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
    lastRequest       = req;
    g_ThreadMgr.savingThread->addRequest(req);
}

void OutputFile::flush()
{
    auto bucket = lastFlushedBucket ? lastFlushedBucket->nextBucket : firstBucket;

    while (bucket)
    {
        flushBucket(bucket);
        bucket = bucket->nextBucket;
    }

    bool done = false;
    while (!done)
    {
        std::unique_lock<std::mutex> lk(mutexNotify);
        condVar.wait(lk);
        done = lastRequest->done;
        for (auto req : reqToRelease)
            g_ThreadMgr.savingThread->releaseRequest(req);
        reqToRelease.clear();
    }
}

void OutputFile::notifySave(SaveThreadRequest* req)
{
    std::unique_lock<std::mutex> lk(mutexNotify);
    condVar.notify_one();
    reqToRelease.push_back(req);
}