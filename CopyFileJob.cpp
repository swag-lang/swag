#include "pch.h"
#include "Module.h"
#include "CopyFileJob.h"
#include "ThreadManager.h"
#include "SourceFile.h"
#include "semanticJob.h"

Pool<CopyFileJob> g_Pool_copyFileJob;

JobResult CopyFileJob::execute()
{
    // Copy only if source is more recent than destination
    if (fs::exists(destPath))
    {
        auto   timeSrc  = fs::last_write_time(sourcePath);
        time_t tsrc     = fs::file_time_type::clock::to_time_t(timeSrc);
        auto   timeDest = fs::last_write_time(destPath);
        time_t tdest    = fs::file_time_type::clock::to_time_t(timeDest);
        if (tdest > tsrc)
        {
            if (g_CommandLine.verboseBuildPass)
                g_Log.verbose(format("   module '%s', file '%s' is up to date", module->name.c_str(), sourcePath.c_str()));
            return JobResult::ReleaseJob;
        }
    }

    FILE* fsrc  = nullptr;
    FILE* fdest = nullptr;
    fopen_s(&fsrc, sourcePath.c_str(), "rb");
    fopen_s(&fdest, destPath.c_str(), "wb");
    SWAG_ASSERT(fsrc && fdest);

    if (g_CommandLine.verboseBuildPass)
        g_Log.verbose(format("   module '%s', copy file '%s' to '%s'", module->name.c_str(), sourcePath.c_str(), destPath.c_str()));

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
    fclose(fdest);

    return JobResult::ReleaseJob;
}
