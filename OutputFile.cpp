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

bool OutputFile::openWrite()
{
    if (fileHandle != nullptr)
        return true;

    if (!openedOnce)
        File::openFile(&fileHandle, path.c_str(), "wtN");
    else
        File::openFile(&fileHandle, path.c_str(), "a+tN");
    if (fileHandle == nullptr)
        return false;

    openedOnce = true;
    return true;
}

bool OutputFile::flush(bool last)
{
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
