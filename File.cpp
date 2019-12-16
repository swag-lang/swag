#include "pch.h"
#include "File.h"
#include "ThreadManager.h"
#include "IoThread.h"
#include "Diagnostic.h"
#include "Workspace.h"
#include "File.h"

bool File::openRead()
{
    if (fileHandle != nullptr)
        return true;
    openedOnce = true;

    // Seems that we need 'N' flag to avoid handle to be shared with spawned processes
    IoThread::openFile(&fileHandle, path.string().c_str(), "rbN");
    if (fileHandle == nullptr)
        return false;
    setvbuf(fileHandle, nullptr, _IONBF, 0);
    return true;
}

void File::close()
{
    IoThread::closeFile(&fileHandle);
}