#include "pch.h"
#include "File.h"
#include "Diagnostic.h"
#include "File.h"
#include "Stats.h"
#include "SourceFile.h"
#include "OutputFile.h"

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

bool File::openWrite()
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

void File::close()
{
    File::closeFile(&fileHandle);
}

void File::save(SaveRequest* request)
{
    if (request->file->openWrite())
    {
        fwrite(request->buffer, 1, request->bufferSize, request->file->fileHandle);
    }
}

void File::load(LoadRequest* request)
{
    request->loadedSize = 0;
    if (request->file->openRead())
    {
        request->file->seekTo(request->seek);
        request->loadedSize = request->file->readTo(request->buffer);
    }

    if (g_Stats.maxOpenFiles > _getmaxstdio() / 2)
        request->file->close();
}
