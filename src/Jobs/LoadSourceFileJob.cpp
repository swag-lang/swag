#include "pch.h"
#include "Jobs/LoadSourceFileJob.h"
#include "Wmf/SourceFile.h"

LoadSourceFileJob::LoadSourceFileJob()
{
    addFlag(JOB_IS_IO);
}

void LoadSourceFileJob::release()
{
    Allocator::free<LoadSourceFileJob>(this);
}

JobResult LoadSourceFileJob::execute()
{
    (void) sourceFile->load();
    return JobResult::ReleaseJob;
}
