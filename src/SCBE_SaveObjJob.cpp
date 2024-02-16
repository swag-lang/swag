#include "pch.h"
#include "SCBE_SaveObjJob.h"
#include "Module.h"
#include "ModulePrepOutputJob.h"
#include "SCBE.h"
#ifdef SWAG_STATS
#include "Timer.h"
#endif

JobResult SCBE_SaveObjJob::execute()
{
#ifdef SWAG_STATS
    Timer timer0{&g_Stats.prepOutputStage1TimeJob};
    Timer timer1{&g_Stats.prepOutputTimeJobSaveObj};
#endif

    static_cast<SCBE*>(module->backend)->saveObjFile(prepJob->buildParameters);
    return JobResult::ReleaseJob;
}
