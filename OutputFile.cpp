#include "pch.h"
#include "OutputFile.h"
#include "ThreadManager.h"
#include "SavingThread.h"
#include "Diagnostic.h"

void OutputFile::flushBucket(ConcatBucket* bucket)
{
    lastFlushedBucket = bucket;
    auto req          = g_ThreadMgr.savingThread->newRequest();
    req->file         = this;
    req->buffer       = (char*) bucket->datas;
    req->bufferSize   = bucket->count;
    lastRequest       = req;
    pendingRequests++;
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

    while (true)
    {
        std::unique_lock<std::mutex> lk(mutexNotify);
        if (!pendingRequests)
        {
			SWAG_ASSERT(reqToRelease.empty());
            break;
        }

        condVar.wait(lk);
        for (auto req : reqToRelease)
            g_ThreadMgr.savingThread->releaseRequest(req);
        reqToRelease.clear();
    }

	SWAG_ASSERT(!pendingRequests);
}

void OutputFile::notifySave(SaveThreadRequest* req)
{
    std::unique_lock<std::mutex> lk(mutexNotify);
    reqToRelease.push_back(req);
    pendingRequests--;
    condVar.notify_one();
}