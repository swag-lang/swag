#include "pch.h"
#include "ByteCode.h"
#include "Context.h"
#include "Module.h"
#include "PrepCompilerMsgJob.h"
#include "Semantic.h"
#include "ThreadManager.h"

bool Module::postCompilerMessage(CompilerMessage& msg)
{
    // We can decide to filter the message only if all #message functions have been registered
    if (numCompilerFunctions == 0)
    {
        if (byteCodeCompiler[static_cast<int>(msg.concrete.kind)].empty())
            return true;
    }

    ScopedLock lk(mutexCompilerMessages);
    if (msg.concrete.kind == CompilerMsgKind::AttributeGen)
        compilerMessages[0].push_back(msg);
    else
        compilerMessages[1].push_back(msg);
    return true;
}

bool Module::prepareCompilerMessages(const JobContext* context, uint32_t pass)
{
    // Eliminate messages without a corresponding #message
    for (uint32_t i = 0; i < compilerMessages[pass].size(); i++)
    {
        auto& msg = compilerMessages[pass][i];

        // If no #message function corresponding to the message, remove
        if (byteCodeCompiler[static_cast<int>(msg.concrete.kind)].empty())
        {
            compilerMessages[pass][i] = compilerMessages[pass].back();
            compilerMessages[pass].pop_back();
            i--;
            continue;
        }
    }

    if (compilerMessages[pass].empty())
        return true;

    // Batch prepare
    auto count          = compilerMessages[pass].size() / g_ThreadMgr.numWorkers;
    count               = max(count, 1);
    count               = min(count, compilerMessages[pass].size());
    uint32_t startIndex = 0;
    while (startIndex < compilerMessages[pass].size())
    {
        const auto newJob    = Allocator::alloc<PrepCompilerMsgJob>();
        newJob->module       = this;
        newJob->pass         = pass;
        newJob->startIndex   = startIndex;
        newJob->endIndex     = min(startIndex + count, compilerMessages[pass].size());
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

    logStage("#message flush\n");

    for (auto& msg : compilerMessages[pass])
    {
        SWAG_ASSERT(!byteCodeCompiler[static_cast<int>(msg.concrete.kind)].empty());

        // If we have a type, then the concrete type should have been generated
        if (!msg.typeInfo || msg.concrete.type)
            sendCompilerMessage(&msg.concrete, context->baseJob);

        // Release symbol
        if (msg.concrete.kind == CompilerMsgKind::AttributeGen)
        {
            SWAG_ASSERT(msg.node->resolvedSymbolName());
            ScopedLock lk(msg.node->resolvedSymbolName()->mutex);
            msg.node->resolvedSymbolName()->flags.remove(SYMBOL_ATTRIBUTE_GEN);
            msg.node->resolvedSymbolName()->dependentJobs.setRunning();
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
    if (byteCodeCompiler[static_cast<int>(msg->kind)].empty())
        return true;

    // Convert to a concrete message for the user
    msg->moduleName.buffer = static_cast<void*>(const_cast<char*>(name.c_str()));
    msg->moduleName.count  = name.length();

    // Find to do that, as this function can only be called once (no multithreading)
    g_RunContext->currentCompilerMessage = msg;

    JobContext context;
    context.baseJob = dependentJob;

    // Push a copy of the default context, in case the user code changes it (or push a new one)
    // :PushDefaultCxt
    PushSwagContext cxt;

    for (const auto bc : byteCodeCompiler[static_cast<int>(msg->kind)])
    {
        SWAG_CHECK(executeNode(bc->node->token.sourceFile, bc->node, &context));
    }

    g_RunContext->currentCompilerMessage = nullptr;
    return true;
}
