#include "pch.h"
#include "File.h"
#include "FileJob.h"
#include "Os.h"
#include "Module.h"

JobResult CopyFileJob::execute()
{
    // Copy only if source is more recent than destination
    if (fs::exists(destPath))
    {
        auto tsrc  = OS::getFileWriteTime(sourcePath.c_str());
        auto tdest = OS::getFileWriteTime(destPath.c_str());
        if (tdest > tsrc)
            return JobResult::ReleaseJob;
    }

    FILE* fsrc  = nullptr;
    FILE* fdest = nullptr;
    if (!openFile(&fsrc, sourcePath.c_str(), "rbN") || !openFile(&fdest, destPath.c_str(), "wbN"))
    {
        module->numErrors++;
        closeFile(&fsrc);
        closeFile(&fdest);
        return JobResult::ReleaseJob;
    }

    uint8_t buffer[4096];
    while (true)
    {
        auto numRead = fread(buffer, 1, sizeof(buffer), fsrc);
        if (numRead)
            fwrite(buffer, 1, numRead, fdest);
        if (numRead != sizeof(buffer))
            break;
    }

    closeFile(&fsrc);
    closeFile(&fdest);

    return JobResult::ReleaseJob;
}

JobResult LoadFileJob::execute()
{
    FILE* fsrc = nullptr;
    if (!openFile(&fsrc, sourcePath.c_str(), "rbN"))
        return JobResult::ReleaseJob;
    fread(destBuffer, 1, sizeBuffer, fsrc);
    closeFile(&fsrc);
    return JobResult::ReleaseJob;
}