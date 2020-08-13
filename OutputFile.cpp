#include "pch.h"
#include "OutputFile.h"
#include "ThreadManager.h"
#include "Log.h"
#include "Os.h"

bool OutputFile::openWrite()
{
    if (winHandle != INVALID_HANDLE_VALUE)
        return true;
    SWAG_ASSERT(!openedOnce);

    winHandle = ::CreateFileA(path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);
    if (winHandle == INVALID_HANDLE_VALUE)
    {
        g_Log.error(format("error opening file '%s' for writing: '%s'", path.c_str(), OS::getLastErrorAsString().c_str()));
        return false;
    }

    openedOnce = true;
    return true;
}

bool OutputFile::flush(bool last, const function<bool(Job*)>& execAsync)
{
    auto bucket = firstBucket;
    while (bucket != lastBucket->nextBucket)
    {
        save(bucket, bucketCount(bucket), execAsync);
        bucket = bucket->nextBucket;
    }

    if (last)
    {
        SWAG_ASSERT(winHandle != INVALID_HANDLE_VALUE);
        ::CloseHandle(winHandle);
    }

    clear();
    return true;
}

void OutputFile::save(ConcatBucket* bucket, uint32_t count, const function<bool(Job*)>& execAsync)
{
    if (!openWrite())
        return;

    OVERLAPPED* over = (OVERLAPPED*) g_Allocator.alloc(sizeof(OVERLAPPED));
    memset(over, 0, sizeof(OVERLAPPED));
    over->Offset = seekSave;
    overlappeds.push_back(over);

    auto result = ::WriteFile(winHandle, bucket->datas, count, NULL, over);
    seekSave += count;

    if (!result)
    {
        auto err = GetLastError();
        if (err != ERROR_IO_PENDING)
        {
            g_Log.error(format("error writing to file '%s': '%s'", path.c_str(), OS::getLastErrorAsString().c_str()));
        }
        else
        {
            DWORD written = 0;
            while (!GetOverlappedResult(winHandle, over, &written, false))
            {
                if (execAsync)
                    g_ThreadMgr.participate(execAsync);
            }
        }
    }
}
