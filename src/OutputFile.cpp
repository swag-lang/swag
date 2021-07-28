#include "pch.h"
#include "OutputFile.h"
#include "ThreadManager.h"
#include "Log.h"
#include "ErrorIds.h"

bool OutputFile::openWrite()
{
    if (winHandle != INVALID_HANDLE_VALUE)
        return true;

    winHandle = ::CreateFileA(path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);
    if (winHandle == INVALID_HANDLE_VALUE)
    {
        g_Log.errorOS(Utf8::format(Msg0524, path.c_str()));
        return false;
    }

    return true;
}

void OutputFile::close()
{
    if (winHandle == INVALID_HANDLE_VALUE)
        return;
    ::CloseHandle(winHandle);
    winHandle = INVALID_HANDLE_VALUE;
}

bool OutputFile::flush(bool last)
{
    auto bucket = firstBucket;
    while (bucket != lastBucket->nextBucket)
    {
        save(bucket->datas, bucketCount(bucket));
        bucket = bucket->nextBucket;
    }

    if (last)
        close();

    clear();
    return true;
}

bool OutputFile::save(void* buffer, uint32_t count)
{
    if (!count)
        return true;
    if (!openWrite())
        return false;

    auto over = (OVERLAPPED*) g_Allocator.alloc(sizeof(OVERLAPPED));
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
            g_Log.errorOS(Utf8::format(Msg0525, path.c_str()));
            return false;
        }

        DWORD written = 0;
        while (!GetOverlappedResult(winHandle, over, &written, false))
        {
            g_ThreadMgr.participate();
        }
    }

    return true;
}
