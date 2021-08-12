#include "pch.h"
#include "File.h"
#include "CopyFileJob.h"
#include "Os.h"

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