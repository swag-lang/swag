#include "pch.h"
#include "Jobs/PrepCompilerMsgJob.h"
#include "Threading/JobThread.h"
#include "Wmf/Module.h"

JobResult PrepCompilerMsgJob::execute()
{
    if (!module)
        return JobResult::ReleaseJob;

    JobContext context;
    context.sourceFile = module->files.front();
    context.node       = context.sourceFile->astRoot; // @JobNodeIsFile
    context.baseJob    = this;
    baseContext        = &context;

    bool mustWait = false;
    for (uint32_t i = startIndex; i < endIndex; i++)
    {
        auto& msg = module->compilerMessages[pass][i];
        SWAG_ASSERT(!module->byteCodeCompiler[static_cast<int>(msg.concrete.kind)].empty());

        if (msg.typeInfo && !msg.concrete.type)
        {
            // As we give to the user the pointer to the type, we can store it wherever we want.
            // So we store the concrete type in a specific 'per thread' segment to avoid contention
            const auto storageSegment = context.sourceFile->module->compilerSegmentPerThread[g_ThreadIndex];
            uint32_t   storageOffset;

            context.result = ContextResult::Done;

            GenExportFlags makeFlags = GEN_EXPORTED_TYPE_SHOULD_WAIT;
            if (pass == 0)
                makeFlags.add(GEN_EXPORTED_TYPE_PARTIAL);

            if (!module->typeGen.genExportedTypeInfo(&context, msg.typeInfo, storageSegment, &storageOffset, makeFlags))
                return JobResult::ReleaseJob;
            if (context.result != ContextResult::Done)
                mustWait = true;

            msg.concrete.type = reinterpret_cast<ExportedTypeInfo*>(storageSegment->address(storageOffset));
        }
    }

    if (mustWait)
        return JobResult::KeepJobAlive;
    return JobResult::ReleaseJob;
}
