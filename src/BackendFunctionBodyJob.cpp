#include "pch.h"
#include "BackendFunctionBodyJob.h"
#include "Backend.h"
#include "ByteCode.h"
#include "Timer.h"

JobResult BackendFunctionBodyJob::execute()
{
#ifdef SWAG_STATS
    Timer timer0{&g_Stats.prepOutputStage1TimeJob};
    Timer timer1{&g_Stats.prepOutputTimeJob_GenFunc};
#endif

    for (auto one : byteCodeFunc)
    {
        if (one->node && one->node->attributeFlags & (ATTRIBUTE_MIXIN | ATTRIBUTE_MACRO | ATTRIBUTE_COMPILER))
            continue;

        // Emit the internal function
        if (!backend->emitFunctionBody(buildParameters, module, one))
            return JobResult::ReleaseJob;
    }

    return JobResult::ReleaseJob;
}
