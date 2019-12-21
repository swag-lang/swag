#include "pch.h"
#include "OutputFile.h"
#include "ThreadManager.h"
#include "Log.h"

void OutputFile::flushBucket(ConcatBucket* bucket)
{
    SaveRequest req;
    req.buffer     = (char*) bucket->datas;
    req.bufferSize = bucket->count;
    save(&req);
}

bool OutputFile::flush(bool last)
{
    bool result = true;
    auto bucket = firstBucket;
    while (bucket)
    {
        flushBucket(bucket);
        bucket = bucket->nextBucket;
    }

    if (last)
        close();
    clear();
    return true;
}

void OutputFile::save(SaveRequest* request)
{
    if (openWrite())
    {
        fwrite(request->buffer, 1, request->bufferSize, fileHandle);
    }
}
