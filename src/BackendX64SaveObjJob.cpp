#include "pch.h"
#include "Module.h"
#include "BackendX64SaveObjJob.h"
#include "ModulePrepOutputJob.h"
#include "BackendX64.h"
#include "Timer.h"

thread_local Pool<BackendX64SaveObjJob> g_Pool_backendX64SaveObjJob;

JobResult BackendX64SaveObjJob::execute()
{
    Timer timer0{&g_Stats.prepOutputTimeJob};
    Timer timer1{&g_Stats.prepOutputTimeJob_SaveObj};
    timer0.start();
    timer1.start();

    ((BackendX64*) module->backend)->saveObjFile(prepJob->buildParameters);

    timer1.stop();
    timer0.stop();
    return JobResult::ReleaseJob;
}
