#include "pch.h"
#include "Backend/SCBE/Obj/SCBE_SaveObjJob.h"
#include "Backend/SCBE/Main/SCBE.h"
#include "Jobs/ModulePrepOutputJob.h"
#include "Wmf/Module.h"
#ifdef SWAG_STATS
#include "Core/Timer.h"
#include "Main/Statistics.h"
#endif

JobResult SCBE_SaveObjJob::execute()
{
#ifdef SWAG_STATS
    Timer timer0{&g_Stats.prepOutputStage1TimeJob};
    Timer timer1{&g_Stats.prepOutputTimeJobSaveObj};
#endif

    reinterpret_cast<SCBE*>(module->backend)->saveObjFile(prepJob->buildParameters);
    return JobResult::ReleaseJob;
}
