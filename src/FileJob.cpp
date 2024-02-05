#include "pch.h"
#include "FileJob.h"
#include "ErrorIds.h"
#include "Module.h"
#include "Os.h"
#include "Report.h"

JobResult CopyFileJob::execute()
{
    // Copy only if source is more recent than destination
    error_code err;
    if (filesystem::exists(destPath, err))
    {
        const auto tsrc  = OS::getFileWriteTime(sourcePath.string().c_str());
        const auto tdest = OS::getFileWriteTime(destPath.string().c_str());
        if (tdest > tsrc)
            return JobResult::ReleaseJob;
    }

    FILE* fsrc  = nullptr;
    FILE* fdest = nullptr;
    if (fopen_s(&fsrc, sourcePath.string().c_str(), "rbN"))
    {
        module->numErrors++;
        Report::errorOS(FMT(Err(Err0095), sourcePath.string().c_str()));
        return JobResult::ReleaseJob;
    }

    if (fopen_s(&fdest, destPath.string().c_str(), "wbN"))
    {
        module->numErrors++;
        fclose(fsrc);
        Report::errorOS(FMT(Err(Err0095), destPath.string().c_str()));
        return JobResult::ReleaseJob;
    }

    uint8_t buffer[4096];
    while (true)
    {
        const auto numRead = fread(buffer, 1, sizeof(buffer), fsrc);
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
        Report::errorOS(FMT(Err(Err0095), sourcePath.string().c_str()));
        return JobResult::ReleaseJob;
    }

    fread(destBuffer, 1, sizeBuffer, fsrc);
    fclose(fsrc);
    return JobResult::ReleaseJob;
}
