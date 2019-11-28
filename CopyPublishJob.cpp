#include "pch.h"
#include "Module.h"
#include "CopyPublishJob.h"
#include "ThreadManager.h"
#include "SourceFile.h"
#include "semanticJob.h"

Pool<CopyPublishJob> g_Pool_copyPublishJob;

JobResult CopyPublishJob::execute()
{
    return JobResult::ReleaseJob;
}
