#include "pch.h"
#include "LoadSourceFileJob.h"
#include "SourceFile.h"

JobResult LoadSourceFileJob::execute()
{
    sourceFile->load();
    return JobResult::ReleaseJob;
}