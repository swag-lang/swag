#include "pch.h"
#include "Module.h"
#include "CopyFileJob.h"
#include "ThreadManager.h"
#include "SourceFile.h"
#include "semanticJob.h"
#include "OS.h"
#include "IoThread.h"

thread_local Pool<CopyFileJob> g_Pool_copyFileJob;

JobResult CopyFileJob::execute()
{
    // Copy only if source is more recent than destination
    if (fs::exists(destPath))
    {
        auto tsrc  = OS::getFileWriteTime(sourcePath);
        auto tdest = OS::getFileWriteTime(destPath);
        if (tdest > tsrc)
        {
            if (g_CommandLine.verboseBuildPass)
                g_Log.verbose(format("   module '%s', file '%s' is up to date", module->name.c_str(), sourcePath.c_str()));
            return JobResult::ReleaseJob;
        }
    }

    FILE* fsrc  = nullptr;
    FILE* fdest = nullptr;
    IoThread::openFile(&fsrc, sourcePath.c_str(), "rbN");
    IoThread::openFile(&fdest, destPath.c_str(), "wbN");
    if (!fsrc || !fdest)
    {
        IoThread::closeFile(&fsrc);
        IoThread::closeFile(&fdest);
        return JobResult::ReleaseJob;
    }

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

    IoThread::closeFile(&fsrc);
    IoThread::closeFile(&fdest);

    return JobResult::ReleaseJob;
}
