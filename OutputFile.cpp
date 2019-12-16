#include "pch.h"
#include "OutputFile.h"
#include "ThreadManager.h"
#include "Log.h"

void OutputFile::flushBucket(ConcatBucket* bucket)
{
    SaveRequest req;
    lastFlushedBucket = bucket;
    req.buffer        = (char*) bucket->datas;
    req.bufferSize    = bucket->count;
    save(&req);
}

bool OutputFile::flush(bool last)
{
    bool result = true;
    auto bucket = lastFlushedBucket ? lastFlushedBucket->nextBucket : firstBucket;
    while (bucket)
    {
        flushBucket(bucket);
        bucket = bucket->nextBucket;
    }

    lastOne           = last;
    lastFlushedBucket = nullptr;
    if (last)
        close();
    clear();
    return result;
}

void OutputFile::save(SaveRequest* request)
{
    if (openWrite())
    {
        fwrite(request->buffer, 1, request->bufferSize, fileHandle);
    }
}
