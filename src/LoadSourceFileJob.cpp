#include "pch.h"
#include "LoadSourceFileJob.h"
#include "SourceFile.h"

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
	sourceFile->load();
	return JobResult::ReleaseJob;
}
