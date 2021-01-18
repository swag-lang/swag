#include "pch.h"
#include "ModulePrepOutputJob.h"
#include "Backend.h"
#include "Workspace.h"
#include "Timer.h"
#include "Module.h"
#include "Stats.h"

thread_local Pool<ModulePrepOutputJob> g_Pool_modulePrepOutputJob;

JobResult ModulePrepOutputJob::execute()
{
    Timer timer{&g_Stats.prepOutputTimeJob};

    timer.start();
    auto result = module->backend->prepareOutput(buildParameters, this);
    timer.stop();

    return result;
}
