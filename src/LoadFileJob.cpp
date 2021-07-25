#include "pch.h"
#include "Module.h"
#include "File.h"
#include "LoadFileJob.h"
#include "OS.h"

thread_local Pool<LoadFileJob> g_Pool_loadFileJob;

JobResult LoadFileJob::execute()
{
    FILE* fsrc = nullptr;
    File::openFile(&fsrc, sourcePath.c_str(), "rbN");
    if (!fsrc)
    {
        File::closeFile(&fsrc);
        return JobResult::ReleaseJob;
    }

    fread(destBuffer, 1, sizeBuffer, fsrc);
    File::closeFile(&fsrc);

    return JobResult::ReleaseJob;
}