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
        SWAG_ASSERT(!module->byteCodeCompiler[(int) msg.kind].empty());

        if (msg.kind == CompilerMsgKind::SemanticFunc)
        {
            if (msg.flags & CM_TYPE_CONVERTED)
                continue;

            auto     storageSegment = &context.sourceFile->module->compilerSegment;
            uint32_t storageOffset;

            context.result = ContextResult::Done;

            if (!module->typeTable.makeConcreteTypeInfo(&context, (TypeInfo*) msg.type, storageSegment, &storageOffset, MAKE_CONCRETE_SHOULD_WAIT))
                return JobResult::ReleaseJob;
            if (context.result != ContextResult::Done)
                return JobResult::KeepJobAlive;
            msg.flags |= CM_TYPE_CONVERTED;
            msg.type = (ConcreteTypeInfo*) storageSegment->address(storageOffset);
        }
    }

    return JobResult::ReleaseJob;
}
