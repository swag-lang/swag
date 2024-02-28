#include "pch.h"
#include "FileJob.h"
#include "ErrorIds.h"
#include "Module.h"
#include "Os.h"
#include "Report.h"

CopyFileJob::CopyFileJob()
{
    addFlag(JOB_IS_IO);
}

void CopyFileJob::release()
{
    Allocator::free<CopyFileJob>(this);
}

JobResult CopyFileJob::execute()
{
    // Copy only if source is more recent than destination
    std::error_code err;
    if (std::filesystem::exists(destPath, err))
    {
        const auto tsrc  = OS::getFileWriteTime(sourcePath);
        const auto tdest = OS::getFileWriteTime(destPath);
        if (tdest > tsrc)
            return JobResult::ReleaseJob;
    }

    FILE* fsrc  = nullptr;
    FILE* fdest = nullptr;
    if (fopen_s(&fsrc, sourcePath, "rbN"))
    {
        ++module->numErrors;
        Report::errorOS(formErr(Err0095, sourcePath.c_str()));
        return JobResult::ReleaseJob;
    }

    if (fopen_s(&fdest, destPath, "wbN"))
    {
        ++module->numErrors;
        (void) fclose(fsrc);
        Report::errorOS(formErr(Err0095, destPath.c_str()));
        return JobResult::ReleaseJob;
    }

    uint8_t buffer[4096];
    while (true)
    {
        const auto numRead = fread(buffer, 1, sizeof(buffer), fsrc);
        if (numRead)
            (void) fwrite(buffer, 1, numRead, fdest);
        if (numRead != sizeof(buffer))
            break;
    }

    (void) fclose(fsrc);
    (void) fflush(fdest);
    (void) fclose(fdest);

    return JobResult::ReleaseJob;
}

LoadFileJob::LoadFileJob()
{
    addFlag(JOB_IS_IO);
}

JobResult LoadFileJob::execute()
{
    FILE* fsrc = nullptr;
    if (fopen_s(&fsrc, sourcePath, "rbN"))
    {
        Report::errorOS(formErr(Err0095, sourcePath.c_str()));
        return JobResult::ReleaseJob;
    }

    (void) fread(destBuffer, 1, sizeBuffer, fsrc);
    (void) fclose(fsrc);
    return JobResult::ReleaseJob;
}
