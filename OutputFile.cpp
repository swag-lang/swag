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

void OutputFile::close()
{
    if (winHandle == INVALID_HANDLE_VALUE)
        return;
    ::CloseHandle(winHandle);
    winHandle = INVALID_HANDLE_VALUE;
}

bool OutputFile::flush(bool last, uint8_t pendingAffinity)
{
    auto bucket = firstBucket;
    while (bucket != lastBucket->nextBucket)
    {
        save(bucket->datas, bucketCount(bucket), pendingAffinity);
        bucket = bucket->nextBucket;
    }

    if (last)
        close();

    clear();
    return true;
}

bool OutputFile::save(void* buffer, uint32_t count, uint8_t pendingAffinity)
{
    if (!count)
        return true;
    if (!openWrite())
        return false;

    OVERLAPPED* over = (OVERLAPPED*) g_Allocator.alloc(sizeof(OVERLAPPED));
    memset(over, 0, sizeof(OVERLAPPED));
    over->Offset = seekSave;
    overlappeds.push_back(over);

    auto result = ::WriteFile(winHandle, buffer, count, NULL, over);
    seekSave += count;

    if (!result)
    {
        auto err = GetLastError();
        if (err != ERROR_IO_PENDING)
        {
            g_Log.error(format("error writing to file '%s': '%s'", path.c_str(), OS::getLastErrorAsString().c_str()));
            return false;
        }

        DWORD written = 0;
        while (!GetOverlappedResult(winHandle, over, &written, false))
        {
            if (pendingAffinity)
                g_ThreadMgr.participate(pendingAffinity);
        }
    }

    return true;
}
