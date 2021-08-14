#include "pch.h"
#include "Module.h"
#include "PrepCompilerMsgJob.h"
#include "SemanticJob.h"

JobResult PrepCompilerMsgJob::execute()
{
    if (!module)
        return JobResult::ReleaseJob;

    JobContext context;
    context.sourceFile = module->files.front();
    context.node       = context.sourceFile->astRoot;
    context.baseJob    = this;
    baseContext        = &context;

    for (int i = startIndex; i < endIndex; i++)
    {
        auto& msg = module->compilerMessages[i];
        SWAG_ASSERT(!module->byteCodeCompiler[(int) msg.concrete.kind].empty());

        if (msg.concrete.kind == CompilerMsgKind::SemanticFunc)
        {
            if (msg.concrete.type)
                continue;

            auto     storageSegment = &context.sourceFile->module->compilerSegment;
            uint32_t storageOffset;

            context.result = ContextResult::Done;
            if (!module->typeTable.makeConcreteTypeInfo(&context, msg.typeInfo, storageSegment, &storageOffset, MAKE_CONCRETE_SHOULD_WAIT))
                return JobResult::ReleaseJob;
            if (context.result != ContextResult::Done)
                return JobResult::KeepJobAlive;

            msg.concrete.type = (ConcreteTypeInfo*) storageSegment->address(storageOffset);
        }
    }

    return JobResult::ReleaseJob;
}
