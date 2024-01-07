#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCodeGen.h"
#include "Ast.h"
#include "ByteCode.h"
#include "Context.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Report.h"
#include "Semantic.h"
#include "ThreadManager.h"
#include "TypeManager.h"

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

JobResult ByteCodeGenJob::leaveJob(AstNode* node)
{
    ByteCodeGen::releaseByteCodeJob(node);
    return JobResult::ReleaseJob;
}

JobResult ByteCodeGenJob::execute()
{
    ScopedLock lkExecute(executeMutex);

    if (!originalNode)
    {
        SWAG_ASSERT(nodes.size() == 1);
        originalNode = nodes.front();

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
        SWAG_ASSERT(originalNode->extension);

        if (sourceFile->isRuntimeFile)
        {
            // Register allocator interface to the default bytecode context
            if (originalNode->token.text == g_LangSpec->name_SystemAllocator)
            {
                auto typeStruct = CastTypeInfo<TypeInfoStruct>(originalNode->typeInfo, TypeInfoKind::Struct);
                context.result  = ContextResult::Done;
                Semantic::waitAllStructInterfaces(this, typeStruct);
                if (context.result == ContextResult::Pending)
                    return JobResult::KeepJobAlive;
                SWAG_ASSERT(typeStruct->interfaces.size() == 1);
                auto itable = sourceFile->module->constantSegment.address(typeStruct->interfaces[0]->offset);
                SWAG_ASSERT(itable);
                SWAG_ASSERT(((void**) itable)[0]);
                g_SystemAllocatorTable = itable;
            }

            if (originalNode->token.text == g_LangSpec->name_DebugAllocator)
            {
                auto typeStruct = CastTypeInfo<TypeInfoStruct>(originalNode->typeInfo, TypeInfoKind::Struct);
                context.result  = ContextResult::Done;
                Semantic::waitAllStructInterfaces(this, typeStruct);
                if (context.result == ContextResult::Pending)
                    return JobResult::KeepJobAlive;
                SWAG_ASSERT(typeStruct->interfaces.size() == 1);
                auto itable = sourceFile->module->constantSegment.address(typeStruct->interfaces[0]->offset);
                SWAG_ASSERT(itable);
                SWAG_ASSERT(((void**) itable)[0]);
                g_DebugAllocatorTable = itable;
            }

            if (g_SystemAllocatorTable && g_DebugAllocatorTable)
            {
                g_DefaultContext.allocator.data   = nullptr;
                g_DefaultContext.allocator.itable = g_SystemAllocatorTable;
            }
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

        if (context.bc)
        {
            EMIT_INST0(&context, ByteCodeOp::End);

            if (originalNode->kind == AstNodeKind::FuncDecl || context.bc->isCompilerGenerated)
            {
#ifdef SWAG_STATS
                g_Stats.numInstructions += context.bc->numInstructions;
#endif

                // Print resulting bytecode
                if (originalNode->attributeFlags & ATTRIBUTE_PRINT_BC && !(originalNode->attributeFlags & ATTRIBUTE_GENERATED_FUNC))
                {
                    ScopedLock lk(module->mutexByteCode);
                    module->byteCodePrintBC.push_back(context.bc);
                }

                if (originalNode->attributeFlags & ATTRIBUTE_PRINT_GEN_BC && !(originalNode->attributeFlags & ATTRIBUTE_GENERATED_FUNC))
                {
                    ByteCodePrintOptions opt;
                    context.bc->print(opt);
                }
            }
        }

        // Byte code is generated (but not yet resolved, as we need all dependencies to be resolved too)
        {
            if (context.bc &&
                context.bc->node &&
                context.bc->node->kind == AstNodeKind::FuncDecl)
            {
                auto funcNode = CastAst<AstFuncDecl>(context.bc->node, AstNodeKind::FuncDecl);

                // Retrieve the persistent registers
                if (funcNode->registerGetContext != UINT32_MAX)
                {
                    context.bc->registerGetContext = funcNode->registerGetContext;
                    ByteCodeGen::freeRegisterRC(&context, context.bc->registerGetContext);
                }
                if (funcNode->registerStoreRR != UINT32_MAX)
                {
                    context.bc->registerStoreRR = funcNode->registerStoreRR;
                    ByteCodeGen::freeRegisterRC(&context, context.bc->registerStoreRR);
                }
            }

            ScopedLock lk(originalNode->mutex);
            ByteCodeGen::getDependantCalls(originalNode, originalNode->extByteCode()->dependentNodes);
            context.dependentNodesTmp = originalNode->extByteCode()->dependentNodes;

            originalNode->semFlags |= SEMFLAG_BYTECODE_GENERATED;
            dependentJobs.setRunning();
        }

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

    // Inform dependencies that everything is done
    ByteCodeGen::releaseByteCodeJob(originalNode);

    // Register function in compiler list, now that we are done
    if (originalNode->attributeFlags & ATTRIBUTE_COMPILER_FUNC)
        module->addCompilerFunc(originalNode->extByteCode()->bc);

    // #ast/#run etc... can have a #[Swag.PrintBc]. We need to print it now, because it's compile time, and the legit
    // pipeline for printing (after bc optimize) will not be called in that case
    if (context.bc && !g_ThreadMgr.debuggerMode)
    {
        if (originalNode->attributeFlags & ATTRIBUTE_PRINT_BC || (originalNode->ownerFct && originalNode->ownerFct->attributeFlags & ATTRIBUTE_PRINT_BC))
        {
            if (originalNode->attributeFlags & ATTRIBUTE_GENERATED_FUNC || originalNode->kind != AstNodeKind::FuncDecl)
            {
                ByteCodePrintOptions opt;
                context.bc->print(opt);
            }
        }
    }

    // Register runtime function type, by name
    if (sourceFile->isRuntimeFile && context.bc)
    {
        ScopedLock lk(sourceFile->module->mutexFile);
        sourceFile->module->mapRuntimeFcts[context.bc->getCallName()] = context.bc;
    }

    if (context.bc &&
        context.bc->node &&
        context.bc->node->kind == AstNodeKind::FuncDecl)
    {
        auto funcNode = CastAst<AstFuncDecl>(context.bc->node, AstNodeKind::FuncDecl);
        if (!funcNode->sourceFile->shouldHaveError)
        {
            // Be sure that every used registers have been released
            if (context.bc->maxReservedRegisterRC > context.bc->availableRegistersRC.size() + context.bc->staticRegs)
            {
                Report::internalError(funcNode, Fmt("function '%s' does not release all registers !", funcNode->token.ctext()));
                if (originalNode->attributeFlags & ATTRIBUTE_PRINT_BC)
                {
                    ByteCodePrintOptions opt;
                    context.bc->print(opt);
                }
            }
            else if (context.bc->maxReservedRegisterRC < context.bc->availableRegistersRC.size())
            {
                Report::internalError(funcNode, Fmt("function '%s' releases too many registers !", funcNode->token.ctext()));
                if (originalNode->attributeFlags & ATTRIBUTE_PRINT_BC)
                {
                    ByteCodePrintOptions opt;
                    context.bc->print(opt);
                }
            }
        }
    }

    return JobResult::ReleaseJob;
}
