#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCodeGen.h"
#include "ByteCode.h"
#include "Module.h"

void ByteCodeGenJob::release()
{
    context.release();
    Allocator::free<ByteCodeGenJob>(this);
}

ByteCodeGenJob* ByteCodeGenJob::newJob(Job* dependentJob, SourceFile* sourceFile, AstNode* root)
{
    auto byteCodeJob          = Allocator::alloc<ByteCodeGenJob>();
    byteCodeJob->sourceFile   = sourceFile;
    byteCodeJob->module       = sourceFile->module;
    byteCodeJob->dependentJob = dependentJob;
    byteCodeJob->nodes.push_back(root);
    return byteCodeJob;
}

JobResult ByteCodeGenJob::leaveJob(AstNode* node)
{
    ByteCodeGen::releaseByteCodeJob(node);
    return JobResult::ReleaseJob;
}

JobResult ByteCodeGenJob::waitForDependenciesGenerated()
{
    VectorNative<AstNode*> done;
    VectorNative<AstNode*> depNodes;
    VectorNative<AstNode*> save;

    while (!context.dependentNodesTmp.empty())
    {
        for (size_t i = 0; i < context.dependentNodesTmp.size(); i++)
        {
            auto node = context.dependentNodesTmp[i];
            SWAG_ASSERT(node->hasExtByteCode() && node->extByteCode()->bc);

            // Wait for the node if not generated
            {
                ScopedLock lk(node->mutex);
                if (!(node->semFlags & SEMFLAG_BYTECODE_GENERATED))
                {
                    waitingKind = JobWaitKind::SemByteCodeGenerated;
                    waitingNode = node;
                    ScopedLock lk1(node->extByteCode()->byteCodeJob->mutexDependent);
                    node->extByteCode()->byteCodeJob->dependentJobs.add(this);
                    return JobResult::KeepJobAlive;
                }
            }

            // Propagate hasForeignFunctionCalls
            if (context.bc)
            {
                SWAG_ASSERT(node->hasExtByteCode() && node->extByteCode()->bc);
                if (node->extByteCode()->bc->hasForeignFunctionCalls)
                    context.bc->hasForeignFunctionCalls = true;
                for (auto const& fmn : node->extByteCode()->bc->hasForeignFunctionCallsModules)
                    context.bc->hasForeignFunctionCallsModules.insert(fmn);
            }
        }

        save = std::move(context.dependentNodesTmp);
        while (save.size())
        {
            auto node = save.get_pop_back();
            if (node == originalNode)
                continue;

            // If my dependent node has already been computed, then just get the result, everything has already
            // been flatten
            {
                SharedLock lk(node->mutex);
                if (node->semFlags & SEMFLAG_BYTECODE_RESOLVED)
                {
                    SWAG_ASSERT(!node->hasExtByteCode() || !node->extByteCode()->byteCodeJob);
                    SWAG_ASSERT(originalNode->hasExtByteCode());
                    for (auto dep : node->extByteCode()->dependentNodes)
                    {
                        SWAG_ASSERT(dep->semFlags & SEMFLAG_BYTECODE_GENERATED);
                        originalNode->extByteCode()->dependentNodes.push_back_once(dep);
                    }

                    continue;
                }
            }

            // Register one level of dependency
            depNodes.clear();
            ByteCodeGen::getDependantCalls(node, depNodes);
            for (auto dep : depNodes)
            {
                if (!done.contains(dep))
                {
                    context.dependentNodesTmp.push_back_once(dep);
                    done.push_back(dep);
                }

                ScopedLock lk1(dep->mutex);
                originalNode->extByteCode()->dependentNodes.push_back_once(dep);
            }
        }
    }

    return JobResult::Continue;
}

JobResult ByteCodeGenJob::execute()
{
    ScopedLock lkExecute(executeMutex);

    if (!originalNode)
    {
        SWAG_ASSERT(nodes.size() == 1);
        originalNode = nodes.front();
        SWAG_ASSERT(originalNode->extension);

        baseContext        = &context;
        context.baseJob    = this;
        context.bc         = originalNode->extByteCode()->bc;
        context.sourceFile = sourceFile;
        context.node       = originalNode;
    }

    if (sourceFile->module->numErrors)
        return leaveJob(originalNode);

    if (pass == Pass::Generate)
    {
        if (sourceFile->isRuntimeFile)
        {
            context.result = ContextResult::Done;
            if (!ByteCodeGen::setupRuntime(&context, originalNode))
                return leaveJob(originalNode);
            if (context.result == ContextResult::Pending)
                return JobResult::KeepJobAlive;
            SWAG_ASSERT(context.result == ContextResult::Done);
        }

        while (!nodes.empty())
        {
            if (sourceFile->numErrors)
                return leaveJob(originalNode);

            auto node      = nodes.back();
            context.node   = node;
            context.result = ContextResult::Done;

            switch (node->bytecodeState)
            {
            case AstNodeResolveState::Enter:
                if (node->hasExtByteCode() && node->extByteCode()->byteCodeBeforeFct)
                {
                    if (!node->extByteCode()->byteCodeBeforeFct(&context))
                        return leaveJob(originalNode);
                    if (context.result == ContextResult::Pending)
                        return JobResult::KeepJobAlive;
                    if (context.result == ContextResult::NewChilds)
                        continue;
                }

                node->bytecodeState = AstNodeResolveState::ProcessingChilds;
                if (!node->hasComputedValue() && !node->childs.empty())
                {
                    if (!(node->flags & AST_NO_BYTECODE_CHILDS) && !(node->flags & AST_NO_BYTECODE))
                    {
                        for (int i = (int) node->childs.size() - 1; i >= 0; i--)
                        {
                            auto child           = node->childs[i];
                            child->bytecodeState = AstNodeResolveState::Enter;
                            nodes.push_back(child);
                        }
                    }

                    break;
                }

            case AstNodeResolveState::ProcessingChilds:
                if (node->flags & AST_NO_BYTECODE)
                {
                    node->bytecodeState = AstNodeResolveState::PostChilds;
                    continue;
                }

                // Computed constexpr value. Just emit the result
                if (node->hasComputedValue())
                {
                    if (!ByteCodeGen::emitComputedValue(&context))
                        return leaveJob(originalNode);
                }
                else if (node->byteCodeFct)
                {
                    if (!node->byteCodeFct(&context))
                        return leaveJob(originalNode);
                    if (context.result == ContextResult::Pending)
                        return JobResult::KeepJobAlive;
                    if (context.result == ContextResult::NewChilds)
                        continue;
                }

                node->bytecodeState = AstNodeResolveState::PostChilds;

            case AstNodeResolveState::PostChilds:
                if (node->flags & AST_NO_BYTECODE)
                {
                    nodes.pop_back();
                    break;
                }

                if (node->hasExtByteCode() && node->extByteCode()->byteCodeAfterFct)
                {
                    if (!node->extByteCode()->byteCodeAfterFct(&context))
                        return leaveJob(originalNode);
                    if (context.result == ContextResult::Pending)
                        return JobResult::KeepJobAlive;
                    if (context.result == ContextResult::NewChilds)
                        continue;
                }
                nodes.pop_back();
                break;

            default:
                break;
            }
        }

        ByteCodeGen::setupByteCodeGenerated(&context, originalNode);
        dependentJobs.setRunning();
        pass = Pass::WaitForDependenciesGenerated;
    }

    // Wait for other dependent nodes to be generated
    // That way we are sure that every one has registered depend nodes, so the full dependency graph is completed
    // for the next pass
    if (pass == Pass::WaitForDependenciesGenerated)
    {
        auto res = waitForDependenciesGenerated();
        if (res != JobResult::Continue)
            return res;
        pass = Pass::ComputeDependenciesResolved;
    }

    ByteCodeGen::setupByteCodeResolved(&context, originalNode);
    return JobResult::ReleaseJob;
}
