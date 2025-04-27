#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/Context.h"
#include "Jobs/PrepCompilerMsgJob.h"
#include "Semantic/Semantic.h"
#include "Threading/ThreadManager.h"
#include "Wmf/Module.h"

bool Module::postCompilerMessage(CompilerMessage& msg)
{
    // We can decide to filter the message only if all #message functions have been registered
    if (numCompilerFunctions == 0)
    {
        const auto type = static_cast<int>(msg.concrete.kind);
        ScopedLock lk(byteCodeCompilerMutex[type]);
        if (byteCodeCompiler[type].empty())
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
        const auto type = static_cast<int>(msg.concrete.kind);
        if (byteCodeCompiler[type].empty())
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
    count               = std::max(count, static_cast<uint32_t>(1));
    count               = std::min(count, compilerMessages[pass].size());
    uint32_t startIndex = 0;
    while (startIndex < compilerMessages[pass].size())
    {
        const auto newJob    = Allocator::alloc<PrepCompilerMsgJob>();
        newJob->module       = this;
        newJob->pass         = pass;
        newJob->startIndex   = startIndex;
        newJob->endIndex     = std::min(startIndex + count, compilerMessages[pass].size());
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

    if (isNot(ModuleKind::Config))
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
            const auto symbolName = msg.node->resolvedSymbolName();
            SWAG_ASSERT(symbolName);
            ScopedLock lk(symbolName->mutex);
            symbolName->flags.remove(SYMBOL_ATTRIBUTE_GEN);
            symbolName->dependentJobs.setRunning();
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
    msg->moduleName.buffer = static_cast<void*>(const_cast<char*>(name.cstr()));
    msg->moduleName.count  = name.length();

    // Find to do that, as this function can only be called once (no multithreading)
    g_RunContext->currentCompilerMessage = msg;

    JobContext context;
    context.baseJob = dependentJob;

    // Push a copy of the default context, in case the user code changes it (or push a new one)
    // @PushDefaultCxt
    PushSwagContext cxt;

    for (const auto bc : byteCodeCompiler[static_cast<int>(msg->kind)])
    {
        SWAG_CHECK(executeNode(bc->node->token.sourceFile, bc->node, &context));
    }

    g_RunContext->currentCompilerMessage = nullptr;
    return true;
}
