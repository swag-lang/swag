#include "pch.h"
#include "OutputFile.h"
#include "ThreadManager.h"
#include "IoThread.h"
#include "Log.h"

void OutputFile::flushBucket(ConcatBucket* bucket, bool flush, bool last)
{
    lastFlushedBucket = bucket;
    auto req          = g_ThreadMgr.ioThread->newSavingRequest();
    req->file         = this;
    req->buffer       = (char*) bucket->datas;
    req->bufferSize   = bucket->count;
    req->firstSave    = firstSave;
    req->flush        = flush;
    req->lastOne      = flush && last;
    firstSave         = false;
    g_ThreadMgr.ioThread->addSavingRequest(req);
}

bool OutputFile::flush(bool last)
{
    bool result = true;
    auto bucket = lastFlushedBucket ? lastFlushedBucket->nextBucket : firstBucket;

    if (bucket)
    {
        while (bucket)
        {
            flushBucket(bucket, bucket->nextBucket ? false : true, last);
            bucket = bucket->nextBucket;
        }

        unique_lock lk(mutexNotify);
        condVar.wait(lk);
    }

    lastFlushedBucket = nullptr;
    clear();
    return result;
}

void OutputFile::notifySave(bool last)
{
    unique_lock lk(mutexNotify);
    done    = true;
    lastOne = last;
    condVar.notify_one();
}