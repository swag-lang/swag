#include "pch.h"
#include "ModuleGenOutputJob.h"
#include "Backend.h"
#include "Module.h"
#include "Statistics.h"
#ifdef SWAG_STATS
#include "Timer.h"
#endif

ModuleGenOutputJob::ModuleGenOutputJob()
{
	addFlag(JOB_IS_IO);
}

JobResult ModuleGenOutputJob::execute()
{
#ifdef SWAG_STATS
	Timer timer(&g_Stats.genOutputTimeJob);
#endif

	if (!module->backend->generateOutput(buildParameters))
		return JobResult::ReleaseJob;

#ifdef SWAG_STATS
	++g_Stats.numGenModules;
#endif

	// Notify we are done
	if (mutexDone)
	{
		ScopedLock lk(*mutexDone);
		condVar->notify_all();
	}

	// The module is built, so notify (we notify before the test)
	if (buildParameters.compileType != Test)
		module->setHasBeenBuilt(BUILDRES_COMPILER);

	return JobResult::ReleaseJob;
}
