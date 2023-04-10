#include "pch.h"
#include "Module.h"
#include "ByteCode.h"
#include "Context.h"
#include "SemanticJob.h"
#include "PrepCompilerMsgJob.h"
#include "ThreadManager.h"

bool Module::postCompilerMessage(JobContext* context, CompilerMessage& msg)
{
    // We can decide to filter the message only if all #message functions have been registered
    if (numCompilerFunctions == 0)
    {
        int index = (int) msg.concrete.kind;
        if (byteCodeCompiler[index].empty())
            return true;
    }

    ScopedLock lk(mutexCompilerMessages);
    if (msg.concrete.kind == CompilerMsgKind::AttributeGen)
        compilerMessages[0].push_back(msg);
    else
        compilerMessages[1].push_back(msg);
    return true;
}

bool Module::prepareCompilerMessages(JobContext* context, uint32_t pass)
{
    // Eliminate messages without a corresponding #message
    for (size_t i = 0; i < compilerMessages[pass].size(); i++)
    {
        auto& msg = compilerMessages[pass][i];

        // If no #message function corresponding to the message, remove
        int index = (int) msg.concrete.kind;
        if (byteCodeCompiler[index].empty())
        {
            compilerMessages[pass][i] = std::move(compilerMessages[pass].back());
            compilerMessages[pass].pop_back();
            i--;
            continue;
        }
    }

    if (compilerMessages[pass].empty())
        return true;

    // Batch prepare
    auto count        = compilerMessages[pass].size() / g_ThreadMgr.numWorkers;
    count             = max(count, 1);
    count             = min(count, compilerMessages[pass].size());
    size_t startIndex = 0;
    while (startIndex < compilerMessages[pass].size())
    {
        auto newJob          = Allocator::alloc<PrepCompilerMsgJob>();
        newJob->module       = this;
        newJob->pass         = pass;
        newJob->startIndex   = (int) startIndex;
        newJob->endIndex     = (int) min(startIndex + count, compilerMessages[pass].size());
        newJob->dependentJob = context->baseJob;
        startIndex += count;
        context->baseJob->jobsToAdd.push_back(newJob);
    }

    return true;
}

bool Module::flushCompilerMessages(JobContext* context, uint32_t pass, Job* job)
{
    if (compilerMessages[pass].empty())
        return true;

    if (kind != ModuleKind::Config)
    {
        if (!waitForDependenciesDone(job))
        {
            context->result = ContextResult::Pending;
            return true;
        }
    }

    if (g_CommandLine.verboseStages)
        logStage("#message flush\n");

    for (auto& msg : compilerMessages[pass])
    {
        SWAG_ASSERT(!byteCodeCompiler[(int) msg.concrete.kind].empty());

        // If we have a type, then the concrete type should have been generated
        if (!msg.typeInfo || msg.concrete.type)
            sendCompilerMessage(&msg.concrete, context->baseJob);

        // Release symbol
        if (msg.concrete.kind == CompilerMsgKind::AttributeGen)
        {
            SWAG_ASSERT(msg.node->resolvedSymbolName);
            ScopedLock lk(msg.node->resolvedSymbolName->mutex);
            msg.node->resolvedSymbolName->flags &= ~SYMBOL_ATTRIBUTE_GEN;
            msg.node->resolvedSymbolName->dependentJobs.setRunning();
        }
    }

    compilerMessages[pass].release();
    return true;
}

bool Module::sendCompilerMessage(CompilerMsgKind msgKind, Job* dependentJob)
{
    ExportedCompilerMessage msg;
    msg.kind = msgKind;
    return sendCompilerMessage(&msg, dependentJob);
}

bool Module::sendCompilerMessage(ExportedCompilerMessage* msg, Job* dependentJob)
{
    if (byteCodeCompiler[(int) msg->kind].empty())
        return true;

    // Convert to a concrete message for the user
    msg->moduleName.buffer = (void*) name.c_str();
    msg->moduleName.count  = name.length();

    // Find to do that, as this function can only be called once (not multi threaded)
    g_RunContext->currentCompilerMessage = msg;

    JobContext context;
    context.baseJob = dependentJob;

    PushSwagContext cxt;
    for (auto bc : byteCodeCompiler[(int) msg->kind])
    {
        SWAG_CHECK(executeNode(bc->node->sourceFile, bc->node, &context));
    }

    g_RunContext->currentCompilerMessage = nullptr;

    return true;
}
