#include "pch.h"
#include "File.h"
#include "Diagnostic.h"
#include "File.h"
#include "ErrorIds.h"
#include "Log.h"

bool openFile(FILE** fileHandle, const char* path, const char* mode)
{
    *fileHandle = nullptr;
    fopen_s(fileHandle, path, mode);
    if (*fileHandle == nullptr)
    {
        g_Log.errorOS(Utf8::format(Msg0502, path));
        return false;
    }

    return true;
}

void closeFile(FILE** fileHandle)
{
    if (!*fileHandle)
        return;
    fclose(*fileHandle);
    *fileHandle = nullptr;
}
