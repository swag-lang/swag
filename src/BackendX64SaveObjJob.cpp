#include "pch.h"
#include "Module.h"
#include "BackendX64SaveObjJob.h"
#include "ModulePrepOutputJob.h"
#include "BackendX64.h"
#include "Timer.h"

JobResult BackendX64SaveObjJob::execute()
{
#ifdef SWAG_STATS
    Timer timer0{&g_Stats.prepOutputStage1TimeJob};
    Timer timer1{&g_Stats.prepOutputTimeJob_SaveObj};
#endif

    ((BackendX64*) module->backend)->saveObjFile(prepJob->buildParameters);

    return JobResult::ReleaseJob;
}
