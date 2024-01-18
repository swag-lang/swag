#include "pch.h"
#include "SCBE_SaveObjJob.h"
#include "SCBE.h"
#include "Module.h"
#include "ModulePrepOutputJob.h"
#include "Timer.h"

JobResult SCBE_SaveObjJob::execute()
{
#ifdef SWAG_STATS
    Timer timer0{&g_Stats.prepOutputStage1TimeJob};
    Timer timer1{&g_Stats.prepOutputTimeJob_SaveObj};
#endif

    ((SCBE*) module->backend)->saveObjFile(prepJob->buildParameters);
    return JobResult::ReleaseJob;
}
