#include "pch.h"
#include "File.h"
#include "Diagnostic.h"
#include "File.h"
#include "Stats.h"
#include "OS.h"
#include "ErrorIds.h"

void File::openFile(FILE** fileHandle, const char* path, const char* mode)
{
    *fileHandle = nullptr;
    fopen_s(fileHandle, path, mode);
    if (*fileHandle == nullptr)
    {
        g_Log.errorOS(format(Msg0502, path));
        return;
    }
}

void File::closeFile(FILE** fileHandle)
{
    if (!*fileHandle)
        return;
    fclose(*fileHandle);
    *fileHandle = nullptr;
}
