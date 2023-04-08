#include "pch.h"
#include "BackendLLVMFunctionBodyJob.h"
#include "BackendLLVM.h"
#include "ByteCode.h"
#include "Module.h"
#include "Timer.h"

JobResult BackendLLVMFunctionBodyJob::execute()
{
#ifdef SWAG_STATS
    Timer timer0{&g_Stats.prepOutputStage1TimeJob};
    Timer timer1{&g_Stats.prepOutputTimeJob_GenFunc};
#endif

    BackendLLVM* bachendLLVM = (BackendLLVM*) backend;

    for (auto one : byteCodeFunc)
    {
        if (one->node && one->node->attributeFlags & (ATTRIBUTE_MIXIN | ATTRIBUTE_MACRO | ATTRIBUTE_COMPILER))
            continue;

        // Emit the function
        if (!bachendLLVM->emitFunctionBody(buildParameters, module, one))
            return JobResult::ReleaseJob;
    }

    return JobResult::ReleaseJob;
}
