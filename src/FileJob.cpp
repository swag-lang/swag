#include "pch.h"
#include "FileJob.h"
#include "Os.h"
#include "Module.h"
#include "Report.h"
#include "Utf8.h"
#include "ErrorIds.h"

JobResult CopyFileJob::execute()
{
    // Copy only if source is more recent than destination
    if (filesystem::exists(destPath))
    {
        auto tsrc  = OS::getFileWriteTime(sourcePath.string().c_str());
        auto tdest = OS::getFileWriteTime(destPath.string().c_str());
        if (tdest > tsrc)
            return JobResult::ReleaseJob;
    }

    FILE* fsrc  = nullptr;
    FILE* fdest = nullptr;
    if (fopen_s(&fsrc, sourcePath.string().c_str(), "rbN"))
    {
        module->numErrors++;
        Report::errorOS(Fmt(Err(Err0502), sourcePath.c_str()));
        return JobResult::ReleaseJob;
    }

    if (fopen_s(&fdest, destPath.string().c_str(), "wbN"))
    {
        module->numErrors++;
        fclose(fsrc);
        Report::errorOS(Fmt(Err(Err0502), destPath.string().c_str()));
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

    fclose(fsrc);
    fflush(fdest);
    fclose(fdest);

    return JobResult::ReleaseJob;
}

JobResult LoadFileJob::execute()
{
    FILE* fsrc = nullptr;
    if (fopen_s(&fsrc, sourcePath.string().c_str(), "rbN"))
    {
        Report::errorOS(Fmt(Err(Err0502), sourcePath.string().c_str()));
        return JobResult::ReleaseJob;
    }

    fread(destBuffer, 1, sizeBuffer, fsrc);
    fclose(fsrc);
    return JobResult::ReleaseJob;
}