#include "pch.h"
#include "BackendSCBEFunctionBodyJob.h"
#include "BackendSCBE.h"
#include "ByteCode.h"
#include "Timer.h"

JobResult BackendSCBEFunctionBodyJob::execute()
{
#ifdef SWAG_STATS
    Timer timer0{&g_Stats.prepOutputStage1TimeJob};
    Timer timer1{&g_Stats.prepOutputTimeJob_GenFunc};
#endif

    BackendSCBE* bachendSCBE = (BackendSCBE*) backend;

    for (auto one : byteCodeFunc)
    {
        if (one->node && one->node->attributeFlags & (ATTRIBUTE_MIXIN | ATTRIBUTE_MACRO | ATTRIBUTE_COMPILER))
            continue;

        // Emit the internal function
        if (!bachendSCBE->emitFunctionBody(buildParameters, module, one))
            return JobResult::ReleaseJob;
    }

    return JobResult::ReleaseJob;
}
