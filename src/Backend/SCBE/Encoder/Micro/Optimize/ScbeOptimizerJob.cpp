#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/Optimize/ScbeOptimizerJob.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"

JobResult ScbeOptimizerJob::execute()
{
    pp->process(*ppCpu);
    return JobResult::ReleaseJob;
}
