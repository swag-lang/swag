#include "pch.h"
#ifdef _WIN32
#include "OutputFileWin32.h"
#include "Os.h"
#include "ThreadManager.h"
#include "Log.h"
#include "ErrorIds.h"

bool OutputFileWin32::openWrite(const string& path)
{
    if (winHandle != INVALID_HANDLE_VALUE)
        return true;

    filePath  = path;
    winHandle = ::CreateFileA(path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);
    if (winHandle == INVALID_HANDLE_VALUE)
    {
        g_Log.errorOS(Utf8::format(Msg0524, path.c_str()));
        return false;
    }

    return true;
}

void OutputFileWin32::close()
{
    if (winHandle == INVALID_HANDLE_VALUE)
        return;
    ::CloseHandle(winHandle);
    winHandle = INVALID_HANDLE_VALUE;
}

bool OutputFileWin32::save(void* buffer, uint32_t count)
{
    if (!count)
        return true;

    auto over = (OVERLAPPED*) g_Allocator.alloc(sizeof(OVERLAPPED));
    memset(over, 0, sizeof(OVERLAPPED));
    over->Offset = seekSave;
    seekSave += count;

    auto result = ::WriteFile(winHandle, buffer, count, NULL, over);

    if (!result)
    {
        auto err = GetLastError();
        if (err != ERROR_IO_PENDING)
        {
            g_Log.errorOS(Utf8::format(Msg0525, filePath.c_str()));
            return false;
        }

        DWORD written = 0;
        while (!GetOverlappedResult(winHandle, over, &written, false))
        {
            g_ThreadMgr.tryExecuteJob();
        }
    }

    return true;
}

#endif