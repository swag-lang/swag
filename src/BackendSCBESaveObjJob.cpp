#include "pch.h"
#include "BackendSCBESaveObjJob.h"
#include "BackendSCBE.h"
#include "Module.h"
#include "ModulePrepOutputJob.h"
#include "Timer.h"

JobResult BackendSCBESaveObjJob::execute()
{
#ifdef SWAG_STATS
    Timer timer0{&g_Stats.prepOutputStage1TimeJob};
    Timer timer1{&g_Stats.prepOutputTimeJob_SaveObj};
#endif

    ((BackendSCBE*) module->backend)->saveObjFile(prepJob->buildParameters);
    return JobResult::ReleaseJob;
}
