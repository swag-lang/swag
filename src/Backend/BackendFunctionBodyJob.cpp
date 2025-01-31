#include "pch.h"
#include "BackendFunctionBodyJob.h"
#include "Backend/Backend.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Syntax/AstNode.h"
#ifdef SWAG_STATS
#include "Core/Timer.h"
#include "Main/Statistics.h"
#endif

JobResult BackendFunctionBodyJob::execute()
{
#ifdef SWAG_STATS
    Timer timer0{&g_Stats.prepOutputStage1TimeJob};
    Timer timer1{&g_Stats.prepOutputTimeJobGenFunc};
#endif

    for (const auto one : byteCodeFunc)
    {
        if (one->node && one->node->hasAttribute(ATTRIBUTE_MIXIN | ATTRIBUTE_MACRO | ATTRIBUTE_COMPILER))
            continue;

        // Emit the internal function
        if (!backend->emitFunctionBody(buildParameters, one))
            return JobResult::ReleaseJob;
    }

    return JobResult::ReleaseJob;
}
