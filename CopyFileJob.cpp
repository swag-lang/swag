#include "pch.h"
#include "Module.h"
#include "IOJob.h"
#include "OS.h"

thread_local Pool<IOJob> g_Pool_ioJob;

JobResult IOJob::execute()
{
    switch (type)
    {
    case IOJobType::CopyFileJob:
        return CopyFile();
    default:
        SWAG_ASSERT(false);
        break;
    }

    return JobResult::ReleaseJob;
}

JobResult IOJob::CopyFile()
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
    File::openFile(&fsrc, sourcePath.c_str(), "rbN");
    File::openFile(&fdest, destPath.c_str(), "wbN");
    if (!fsrc || !fdest)
    {
        File::closeFile(&fsrc);
        File::closeFile(&fdest);
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

    File::closeFile(&fsrc);
    File::closeFile(&fdest);

    return JobResult::ReleaseJob;
}