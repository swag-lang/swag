#include "pch.h"
#include "Module.h"
#include "ByteCode.h"
#include "Context.h"
#include "SemanticJob.h"
#include "PrepCompilerMsgJob.h"

void Module::postCompilerMessage(CompilerMessage& msg)
{
    // We can decide to filter the message only if all #compiler functions have been registered
    if (numCompilerFunctions == 0)
    {
        int index = (int) msg.concrete.kind;
        if (byteCodeCompiler[index].empty())
            return;
    }

    ScopedLock lk(mutexCompilerMessages);
    compilerMessages.push_back(msg);
}

bool Module::prepareCompilerMessages(JobContext* context)
{
    // Eliminate messages without a corresponding #compiler
    for (int i = 0; i < compilerMessages.size(); i++)
    {
        auto& msg = compilerMessages[i];

        int index = (int) msg.concrete.kind;
        if (byteCodeCompiler[index].empty())
        {
            compilerMessages[i] = move(compilerMessages.back());
            compilerMessages.pop_back();
            i--;
            continue;
        }
    }

    if (compilerMessages.empty())
        return true;

    // Batch prepare
    auto count     = (int) compilerMessages.size() / g_Stats.numWorkers;
    count          = max(count, 1);
    count          = min(count, (int) compilerMessages.size());
    int startIndex = 0;
    while (startIndex < compilerMessages.size())
    {
        auto newJob          = g_Allocator.alloc<PrepCompilerMsgJob>();
        newJob->module       = this;
        newJob->startIndex   = startIndex;
        newJob->endIndex     = min(startIndex + count, (int) compilerMessages.size());
        newJob->dependentJob = context->baseJob;
        startIndex += count;
        context->baseJob->jobsToAdd.push_back(newJob);
    }

    return true;
}

bool Module::flushCompilerMessages(JobContext* context)
{
    for (auto& msg : compilerMessages)
    {
        SWAG_ASSERT(!byteCodeCompiler[(int) msg.concrete.kind].empty());
        sendCompilerMessage(&msg.concrete, context->baseJob);
    }

    compilerMessages.clear();
    compilerMessages.shrink_to_fit();
    return true;
}

bool Module::sendCompilerMessage(CompilerMsgKind msgKind, Job* dependentJob)
{
    ConcreteCompilerMessage msg = {0};
    msg.kind                    = msgKind;
    return sendCompilerMessage(&msg, dependentJob);
}

bool Module::sendCompilerMessage(ConcreteCompilerMessage* msg, Job* dependentJob)
{
    SWAG_ASSERT(numCompilerFunctions == 0);
    if (byteCodeCompiler[(int) msg->kind].empty())
        return true;

    // Convert to a concrete message for the user
    msg->moduleName.buffer = (void*) name.c_str();
    msg->moduleName.count  = name.length();

    // Find to do that, as this function can only be called once (not multi threaded)
    g_RunContext.currentCompilerMessage = msg;

    JobContext context;
    context.baseJob = dependentJob;

    PushSwagContext cxt;
    for (auto bc : byteCodeCompiler[(int) msg->kind])
    {
        SWAG_CHECK(executeNode(bc->node->sourceFile, bc->node, &context));
    }

    g_RunContext.currentCompilerMessage = nullptr;
    return true;
}
