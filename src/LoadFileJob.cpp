#include "pch.h"
#include "File.h"
#include "LoadFileJob.h"

thread_local Pool<LoadFileJob> g_Pool_loadFileJob;

JobResult LoadFileJob::execute()
{
    FILE* fsrc = nullptr;
    if (!openFile(&fsrc, sourcePath.c_str(), "rbN"))
        return JobResult::ReleaseJob;
    fread(destBuffer, 1, sizeBuffer, fsrc);
    closeFile(&fsrc);
    return JobResult::ReleaseJob;
}