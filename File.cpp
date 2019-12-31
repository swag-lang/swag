#include "pch.h"
#include "File.h"
#include "Diagnostic.h"
#include "File.h"
#include "Stats.h"

void File::openFile(FILE** fileHandle, const char* path, const char* mode)
{
    *fileHandle = nullptr;
    auto err    = fopen_s(fileHandle, path, mode);
    if (*fileHandle == nullptr)
    {
        char buf[256];
        strerror_s(buf, err);
        g_Log.error(format("error opening file '%s': '%s'", path, buf));
        return;
    }

    g_Stats.numOpenFiles++;
    g_Stats.maxOpenFiles = max(g_Stats.maxOpenFiles.load(), g_Stats.numOpenFiles.load());
}

void File::closeFile(FILE** fileHandle)
{
    if (!*fileHandle)
        return;
    fclose(*fileHandle);
    *fileHandle = nullptr;
    SWAG_ASSERT(g_Stats.numOpenFiles.load());
    g_Stats.numOpenFiles--;
}

bool File::openRead()
{
    if (fileHandle != nullptr)
        return true;

    // Seems that we need 'N' flag to avoid handle to be shared with spawned processes
    File::openFile(&fileHandle, path.c_str(), "rbN");
    if (fileHandle == nullptr)
        return false;

    openedOnce = true;
    return true;
}

void File::close()
{
    File::closeFile(&fileHandle);
}
