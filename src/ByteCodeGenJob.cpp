#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCode.h"
#include "ThreadManager.h"
#include "Module.h"
#include "Ast.h"
#include "TypeManager.h"
#include "Context.h"
#include "LanguageSpec.h"
#include "Mutex.h"
#include "ByteCodeOptimizerJob.h"
#include "Diagnostic.h"
#include "ErrorIds.h"

void ByteCodeGenJob::reserveRegisterRC(ByteCodeGenContext* context, RegisterList& rc, int num)
{
    rc.clear();
    rc.cannotFree = false;
    if (num == 0)
        return;
    if (num == 1)
        rc += reserveRegisterRC(context);
    else
    {
        SWAG_ASSERT(num == 2);
        reserveLinearRegisterRC2(context, rc);
    }
}

void ByteCodeGenJob::sortRegistersRC(ByteCodeGenContext* context)
{
    if (!context->bc->isDirtyRegistersRC)
        return;
    context->bc->isDirtyRegistersRC = false;
    if (context->bc->availableRegistersRC.size() <= 1)
        return;
    sort(context->bc->availableRegistersRC.begin(), context->bc->availableRegistersRC.end(), [](uint32_t a, uint32_t b)
         { return a > b; });
}

void ByteCodeGenJob::freeRegisterRC(ByteCodeGenContext* context, uint32_t rc)
{
#ifdef SWAG_DEV_MODE
    for (auto r : context->bc->availableRegistersRC)
        SWAG_ASSERT(r != rc);
#endif

    context->bc->availableRegistersRC.push_back(rc);
    context->bc->isDirtyRegistersRC = true;
}

uint32_t ByteCodeGenJob::reserveRegisterRC(ByteCodeGenContext* context)
{
    if (!context->bc->availableRegistersRC.empty())
    {
        sortRegistersRC(context);
        auto result = context->bc->availableRegistersRC.back();
        context->bc->availableRegistersRC.pop_back();
        return result;
    }

    return context->bc->maxReservedRegisterRC++;
}

void ByteCodeGenJob::reserveLinearRegisterRC2(ByteCodeGenContext* context, RegisterList& rc)
{
    freeRegisterRC(context, rc);

    auto size = context->bc->availableRegistersRC.size();
    if (size > 1)
    {
        sortRegistersRC(context);
        for (int i = 0; i < size - 1; i++)
        {
            if (context->bc->availableRegistersRC[i] == context->bc->availableRegistersRC[i + 1] + 1)
            {
                rc += context->bc->availableRegistersRC[i + 1];
                rc += context->bc->availableRegistersRC[i];
                context->bc->availableRegistersRC.erase(i, 2);
                return;
            }
        }
    }

    rc += context->bc->maxReservedRegisterRC++;
    rc += context->bc->maxReservedRegisterRC++;
}

void ByteCodeGenJob::transformResultToLinear2(ByteCodeGenContext* context, AstNode* node)
{
    transformResultToLinear2(context, node->resultRegisterRC);
}

void ByteCodeGenJob::transformResultToLinear2(ByteCodeGenContext* context, RegisterList& resultRegisterRC)
{
    bool onlyOne = false;
    if (resultRegisterRC.size() == 1)
    {
        SWAG_ASSERT(!resultRegisterRC.cannotFree);
        onlyOne = true;
        resultRegisterRC += reserveRegisterRC(context);
    }

    if (resultRegisterRC[1] != resultRegisterRC[0] + 1)
    {
        RegisterList r0;
        reserveLinearRegisterRC2(context, r0);
        emitInstruction(context, ByteCodeOp::CopyRBtoRA64, r0[0], resultRegisterRC[0])->flags |= BCI_UNPURE;
        if (!onlyOne)
            emitInstruction(context, ByteCodeOp::CopyRBtoRA64, r0[1], resultRegisterRC[1])->flags |= BCI_UNPURE;
        freeRegisterRC(context, resultRegisterRC);
        resultRegisterRC = r0;
    }
}

void ByteCodeGenJob::truncRegisterRC(ByteCodeGenContext* context, RegisterList& rc, int count)
{
    if (rc.size() == count)
        return;

    SWAG_ASSERT(!rc.cannotFree);

    RegisterList rs;
    for (int i = 0; i < count; i++)
        rs += rc[i];

    if (!rc.cannotFree)
    {
        for (int i = count; i < rc.size(); i++)
            freeRegisterRC(context, rc[i]);
    }

    rs.cannotFree = rc.cannotFree;
    rc            = rs;
}

void ByteCodeGenJob::freeRegisterRC(ByteCodeGenContext* context, RegisterList& rc)
{
    if (rc.cannotFree)
        return;
    for (int i = 0; i < rc.size(); i++)
        freeRegisterRC(context, rc[i]);
    rc.clear();
    rc.cannotFree = false;
}

void ByteCodeGenJob::freeRegisterRC(ByteCodeGenContext* context, AstNode* node)
{
    if (!node)
        return;
    freeRegisterRC(context, node->resultRegisterRC);
    freeRegisterRC(context, node->additionalRegisterRC);
}

void ByteCodeGenJob::ensureCanBeChangedRC(ByteCodeGenContext* context, RegisterList& r0)
{
    if (r0.cannotFree)
    {
        RegisterList re;
        reserveRegisterRC(context, re, r0.size());
        for (int i = 0; i < r0.size(); i++)
            emitInstruction(context, ByteCodeOp::CopyRBtoRA64, re[i], r0[i]);
        freeRegisterRC(context, r0);
        r0 = re;
    }
}

bool ByteCodeGenJob::emitPassThrough(ByteCodeGenContext* context)
{
    auto node = context->node;
    if (node->childs.empty())
        return true;
    auto child = node->childs.back();
    if (node->flags & AST_DISCARD)
        freeRegisterRC(context, child);
    else
        node->resultRegisterRC = child->resultRegisterRC;
    freeRegisterRC(context, child->additionalRegisterRC);
    return true;
}

bool ByteCodeGenJob::skipNodes(ByteCodeGenContext* context, AstNode* node)
{
    node->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;
    auto res = Ast::visit(context, node, [](JobContext* cxt, AstNode* n)
                          {
                              if (n->kind != AstNodeKind::Literal)
                                  return true;
                              if (n->semFlags & AST_SEM_LITERAL_SUFFIX)
                                  return cxt->report(n->childs.front(), Fmt(Err(Err0532), n->childs.front()->token.ctext()));
                              return true; });

    return res;
}

void ByteCodeGenJob::emitDebugLine(ByteCodeGenContext* context, AstNode* node)
{
    if (!node)
        return;
    PushLocation lk(context, &node->token.endLocation);
    emitDebugLine(context);
}

void ByteCodeGenJob::emitDebugLine(ByteCodeGenContext* context)
{
    if (context->sourceFile->module->buildCfg.backendDebugInformations &&
        !context->sourceFile->module->buildCfg.byteCodeOptimize &&
        !context->sourceFile->isScriptFile)
    {
        emitInstruction(context, ByteCodeOp::DebugNop);
    }
}

bool ByteCodeGenJob::emitDebugNop(ByteCodeGenContext* context)
{
    auto node = context->node;
    if (!node->childs.empty())
        node->resultRegisterRC = node->childs.back()->resultRegisterRC;
    emitDebugLine(context, node);
    return true;
}

ByteCodeInstruction* ByteCodeGenJob::emitMakeSegPointer(ByteCodeGenContext* context, DataSegment* storageSegment, uint32_t storageOffset, uint32_t r0)
{
    SWAG_ASSERT(storageSegment);
    switch (storageSegment->kind)
    {
    case SegmentKind::Data:
        return emitInstruction(context, ByteCodeOp::MakeMutableSegPointer, r0, storageOffset);
    case SegmentKind::Bss:
        return emitInstruction(context, ByteCodeOp::MakeBssSegPointer, r0, storageOffset);
    case SegmentKind::Compiler:
        return emitInstruction(context, ByteCodeOp::MakeCompilerSegPointer, r0, storageOffset);
    case SegmentKind::Constant:
        return emitInstruction(context, ByteCodeOp::MakeConstantSegPointer, r0, storageOffset);
    default:
        SWAG_ASSERT(false);
    }

    return nullptr;
}

ByteCodeInstruction* ByteCodeGenJob::emitGetFromSeg(ByteCodeGenContext* context, DataSegment* storageSegment, uint32_t storageOffset, uint32_t r0)
{
    SWAG_ASSERT(storageSegment);
    switch (storageSegment->kind)
    {
    case SegmentKind::Data:
        return emitInstruction(context, ByteCodeOp::GetFromMutableSeg64, r0, storageOffset);
    case SegmentKind::Bss:
        return emitInstruction(context, ByteCodeOp::GetFromBssSeg64, r0, storageOffset);
    case SegmentKind::Compiler:
        return emitInstruction(context, ByteCodeOp::GetFromCompilerSeg64, r0, storageOffset);
    default:
        SWAG_ASSERT(false);
    }

    return nullptr;
}

ByteCodeInstruction* ByteCodeGenJob::emitInstruction(ByteCodeGenContext* context, ByteCodeOp op, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3, const std::source_location location)
{
    AstNode* node = context->node;
    auto     bc   = context->bc;

    if (bc->numInstructions == bc->maxInstructions)
    {
        auto oldSize        = (int) (bc->maxInstructions * sizeof(ByteCodeInstruction));
        bc->maxInstructions = bc->maxInstructions * 2;

        // Evaluate the first number of instructions for a given function.
        // We take the number of ast nodes in the function as a metric.
        // 0.7f is kind of magical, based on various measures.
        // This is to mitigate the number of reallocations, without wasting too much memory.
        if (!bc->maxInstructions && bc->node && bc->node->kind == AstNodeKind::FuncDecl)
        {
            auto funcDecl = CastAst<AstFuncDecl>(bc->node, AstNodeKind::FuncDecl);
            if (funcDecl->nodeCounts)
                bc->maxInstructions = (int) (funcDecl->nodeCounts * 0.8f);
        }

        bc->maxInstructions = max(bc->maxInstructions, 8);
        auto newInstuctions = (ByteCodeInstruction*) g_Allocator.alloc(bc->maxInstructions * sizeof(ByteCodeInstruction));
        memcpy(newInstuctions, bc->out, bc->numInstructions * sizeof(ByteCodeInstruction));
        g_Allocator.free(bc->out, oldSize);
        if (g_CommandLine->stats)
        {
            g_Stats.memInstructions -= oldSize;
            g_Stats.memInstructions += bc->maxInstructions * sizeof(ByteCodeInstruction);
        }

        bc->out = newInstuctions;
    }

    SWAG_ASSERT(bc->out);
    ByteCodeInstruction& ins = bc->out[bc->numInstructions++];
    ins.op                   = op;
    ins.a.u64                = r0;
    ins.b.u64                = r1;
    ins.c.u64                = r2;
    ins.d.u64                = r3;
    ins.flags                = context->instructionsFlags;
    if (context->tryCatchScope)
        ins.flags |= BCI_TRYCATCH;
    ins.node = context->forceNode ? context->forceNode : node;
#if defined SWAG_DEV_MODE
    ins.sourceFile            = location.file_name();
    ins.sourceLine            = location.line();
    static atomic<int> serial = 0;
    ins.serial                = serial++;
    ins.treeNode              = 0;
    ins.crc                   = 0;
#endif

    if (context->noLocation)
        ins.location = nullptr;
    else if (context->forceLocation)
        ins.location = context->forceLocation;
    else
        ins.location = &node->token.startLocation;

    if (ByteCode::isJumpOrDyn(&ins))
        bc->numJumps++;

    // Some operations, like IntrinsicTypeCmp for example, are in fact call to runtime functions.
    // We must be sure that we have enough room on the stack to store the parameters (x64 backend).
    context->bc->maxCallParams = max(context->bc->maxCallParams, g_ByteCodeOpDesc[(int) op].numCallParams);
    return &ins;
}

void ByteCodeGenJob::inherhitLocation(ByteCodeInstruction* inst, AstNode* node)
{
    inst->node = node;
}

void ByteCodeGenJob::askForByteCode(Job* job, AstNode* node, uint32_t flags, ByteCode* caller)
{
    if (!node)
        return;

    auto sourceFile = node->sourceFile;

    // If this is a foreign function, we do not need bytecode
    AstFuncDecl* funcDecl = nullptr;
    if (node->kind == AstNodeKind::FuncDecl)
    {
        funcDecl = CastAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
        if (funcDecl->content && (funcDecl->content->flags & AST_NO_SEMANTIC))
            return;

        if (funcDecl->attributeFlags & ATTRIBUTE_FOREIGN)
        {
            // Need to wait for function full semantic resolve
            if (flags & ASKBC_WAIT_SEMANTIC_RESOLVED)
            {
                SWAG_ASSERT(job);
                job->waitFuncDeclFullResolve(funcDecl);
                if (job->baseContext->result != ContextResult::Done)
                    return;
            }

            ScopedLock lk(node->mutex);
            node->semFlags |= AST_SEM_BYTECODE_GENERATED;
            node->semFlags |= AST_SEM_BYTECODE_RESOLVED;
            return;
        }
    }

    if (job)
    {
        // If true, then this is a simple recursive call
        if (node->extension && node->extension->byteCodeJob == job)
            return;

        // Need to wait for function full semantic resolve
        if (flags & ASKBC_WAIT_SEMANTIC_RESOLVED)
        {
            SWAG_ASSERT(funcDecl);
            job->waitFuncDeclFullResolve(funcDecl);
            if (job->baseContext->result != ContextResult::Done)
                return;
        }
    }

    if (caller)
        caller->dependentCalls.push_back_once(node);

    ScopedLock lk(node->mutex);

    // Need to generate bytecode, if not already done or running
    if (!(node->semFlags & AST_SEM_BYTECODE_GENERATED))
    {
        if (flags & ASKBC_WAIT_DONE)
        {
            SWAG_ASSERT(job);
            job->setPending(nullptr, JobWaitKind::SemByteCodeGenerated, node, nullptr);
        }

        node->allocateExtension();
        auto extension = node->extension;
        if (!extension->byteCodeJob)
        {
            extension->byteCodeJob             = g_Allocator.alloc<ByteCodeGenJob>();
            extension->byteCodeJob->sourceFile = sourceFile;
            extension->byteCodeJob->module     = sourceFile->module;
            if (flags & ASKBC_WAIT_DONE)
                extension->byteCodeJob->dependentJob = job;
            else
                extension->byteCodeJob->dependentJob = job->dependentJob;
            extension->byteCodeJob->context.errorContextStack = job->baseContext->errorContextStack;
            extension->byteCodeJob->nodes.push_back(node);
            if (!extension->bc)
            {
                extension->bc             = g_Allocator.alloc<ByteCode>();
                extension->bc->node       = node;
                extension->bc->sourceFile = node->sourceFile;
            }

            extension->bc->typeInfoFunc = funcDecl ? (TypeInfoFuncAttr*) funcDecl->typeInfo : nullptr;
            if (node->flags & AST_DEFINED_INTRINSIC)
                extension->bc->name = node->token.text;
            else if (node->sourceFile->isRuntimeFile)
                extension->bc->name = node->token.text;
            else
            {
                extension->bc->name = node->ownerScope->getFullName();
                extension->bc->name += ".";
                extension->bc->name += node->token.text;
            }

            if (node->kind == AstNodeKind::FuncDecl)
                sourceFile->module->addByteCodeFunc(node->extension->bc);

            if (flags & ASKBC_WAIT_DONE)
                job->jobsToAdd.push_back(node->extension->byteCodeJob);
            else
                g_ThreadMgr.addJob(node->extension->byteCodeJob);
            return;
        }

        if (flags & ASKBC_WAIT_DONE)
        {
            ScopedLock lk1(extension->byteCodeJob->mutexDependent);
            extension->byteCodeJob->dependentJobs.add(job);
        }

        return;
    }

    if (flags & ASKBC_WAIT_RESOLVED)
    {
        SWAG_ASSERT(job);
        if (!(node->semFlags & AST_SEM_BYTECODE_RESOLVED))
        {
            SWAG_ASSERT(node->extension && node->extension->byteCodeJob);
            node->extension->byteCodeJob->dependentJobs.add(job);
            job->setPending(nullptr, JobWaitKind::AskBcWaitResolve, node, nullptr);
            return;
        }
    }
}

void ByteCodeGenJob::releaseByteCodeJob(AstNode* node)
{
    ScopedLock lk(node->mutex);
    node->semFlags |= AST_SEM_BYTECODE_RESOLVED | AST_SEM_BYTECODE_GENERATED;
    node->extension->byteCodeJob = nullptr;
}

void ByteCodeGenJob::getDependantCalls(AstNode* depNode, VectorNative<AstNode*>& dep)
{
    // Normal function
    if (depNode->extension->bc)
    {
        dep.append(depNode->extension->bc->dependentCalls);
    }

    // Struct: special functions
    else
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(depNode->typeInfo, TypeInfoKind::Struct);
        if (typeStruct->opInit)
            dep.append(typeStruct->opInit->dependentCalls);
        if (typeStruct->opDrop)
            dep.append(typeStruct->opDrop->dependentCalls);
        if (typeStruct->opPostCopy)
            dep.append(typeStruct->opPostCopy->dependentCalls);
        if (typeStruct->opPostMove)
            dep.append(typeStruct->opPostMove->dependentCalls);
    }
}

JobResult ByteCodeGenJob::execute()
{
    ScopedLock lkExecute(executeMutex);

    if (!originalNode)
    {
        SWAG_ASSERT(nodes.size() == 1);
        originalNode = nodes.front();
    }

    if (sourceFile->module->numErrors)
    {
        releaseByteCodeJob(originalNode);
        return JobResult::ReleaseJob;
    }

    baseContext        = &context;
    context.job        = this;
    context.sourceFile = sourceFile;
    context.bc         = originalNode->extension->bc;
    context.node       = originalNode;

    if (pass == Pass::Generate)
    {
        SWAG_ASSERT(originalNode->extension);
        SWAG_ASSERT(originalNode->extension->byteCodeJob);

        // Register SystemAllocator interface to the default bytecode context
        if (sourceFile->isRuntimeFile && (originalNode->token.text == g_LangSpec->name_SystemAllocator))
        {
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(originalNode->typeInfo, TypeInfoKind::Struct);
            context.result  = ContextResult::Done;
            waitAllStructInterfaces(typeStruct);
            if (context.result == ContextResult::Pending)
                return JobResult::KeepJobAlive;
            SWAG_ASSERT(typeStruct->interfaces.size() == 1);
            auto itable = sourceFile->module->constantSegment.address(typeStruct->interfaces[0]->offset);
            SWAG_ASSERT(itable);
            SWAG_ASSERT(((void**) itable)[0]);
            g_DefaultContext.allocator.data   = nullptr;
            g_DefaultContext.allocator.itable = (void*) itable;
        }

        while (!nodes.empty())
        {
            if (sourceFile->numErrors)
            {
                releaseByteCodeJob(originalNode);
                return JobResult::ReleaseJob;
            }

            auto node = nodes.back();
            SWAG_CHECK_BLOCK(node);

            context.node   = node;
            context.result = ContextResult::Done;

            switch (node->bytecodeState)
            {
            case AstNodeResolveState::Enter:
                if (node->extension && node->extension->byteCodeBeforeFct && !node->extension->byteCodeBeforeFct(&context))
                {
                    releaseByteCodeJob(originalNode);
                    return JobResult::ReleaseJob;
                }

                if (context.result == ContextResult::Pending)
                    return JobResult::KeepJobAlive;
                if (context.result == ContextResult::NewChilds)
                    continue;

                node->bytecodeState = AstNodeResolveState::ProcessingChilds;
                if (!(node->flags & AST_VALUE_COMPUTED) && !node->childs.empty())
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
                if (!(node->flags & AST_NO_BYTECODE))
                {
                    // Computed constexpr value. Just emit the result
                    if (node->flags & AST_VALUE_COMPUTED)
                    {
                        context.node = node;
                        if (!emitLiteral(&context))
                        {
                            releaseByteCodeJob(originalNode);
                            return JobResult::ReleaseJob;
                        }

                        if (!emitCast(&context, node, TypeManager::concreteType(node->typeInfo), node->castedTypeInfo))
                        {
                            releaseByteCodeJob(originalNode);
                            return JobResult::ReleaseJob;
                        }

                        // To be sure that cast is treated once
                        node->castedTypeInfo = nullptr;
                        SWAG_ASSERT(context.result == ContextResult::Done);
                    }
                    else if (node->byteCodeFct)
                    {
                        if (!node->byteCodeFct(&context))
                        {
                            releaseByteCodeJob(originalNode);
                            return JobResult::ReleaseJob;
                        }

                        if (context.result == ContextResult::Pending)
                            return JobResult::KeepJobAlive;
                        if (context.result == ContextResult::NewChilds)
                            continue;
                    }

                    node->bytecodeState = AstNodeResolveState::PostChilds;
                }

            case AstNodeResolveState::PostChilds:
                if (!(node->flags & AST_NO_BYTECODE))
                {
                    if (node->extension && node->extension->byteCodeAfterFct)
                    {
                        if (!node->extension->byteCodeAfterFct(&context))
                        {
                            releaseByteCodeJob(originalNode);
                            return JobResult::ReleaseJob;
                        }

                        if (context.result == ContextResult::Pending)
                            return JobResult::KeepJobAlive;
                        if (context.result == ContextResult::NewChilds)
                            continue;
                    }
                }

                nodes.pop_back();
                break;
            }
        }

        if (context.bc)
        {
            emitInstruction(&context, ByteCodeOp::End);

            if (originalNode->kind == AstNodeKind::FuncDecl || context.bc->isCompilerGenerated)
            {
                if (g_CommandLine->stats)
                    g_Stats.numInstructions += context.bc->numInstructions;

                // Print resulting bytecode
                if (originalNode->attributeFlags & ATTRIBUTE_PRINT_BC && !(originalNode->attributeFlags & ATTRIBUTE_GENERATED_FUNC))
                {
                    ScopedLock lk(module->mutexByteCode);
                    module->byteCodePrintBC.push_back(context.bc);
                }
            }
        }

        // Byte code is generated (but not yet resolved, as we need all dependencies to be resolved too)
        {
            // Make one optimization
            if (context.bc &&
                context.bc->node &&
                context.bc->node->kind == AstNodeKind::FuncDecl)
            {
                // Retrieve the persistent register used to store the current context
                auto funcNode = CastAst<AstFuncDecl>(context.bc->node, AstNodeKind::FuncDecl);
                if (funcNode->registerGetContext != UINT32_MAX)
                {
                    context.bc->registerGetContext = funcNode->registerGetContext;
                    freeRegisterRC(&context, context.bc->registerGetContext);
                }

                if (!funcNode->sourceFile->numTestErrors)
                {
                    if (sourceFile->module->kind != ModuleKind::BootStrap && sourceFile->module->kind != ModuleKind::Runtime)
                    {
                        ByteCodeOptimizerJob opt;
                        bool                 restart;
                        opt.module = module;
                        opt.optimize(context.bc, restart);
                    }
                }
            }

            ScopedLock lk(originalNode->mutex);
            SWAG_ASSERT(originalNode->extension && originalNode->extension->byteCodeJob);
            getDependantCalls(originalNode, dependentNodes);
            dependentNodesTmp = dependentNodes;

            originalNode->semFlags |= AST_SEM_BYTECODE_GENERATED;
            dependentJobs.setRunning();
        }

        pass = Pass::WaitForDependenciesGenerated;
    }

    // Wait for other dependent nodes to be generated
    // That way we are sure that every one has registered depend nodes, so the full dependency graph is completed
    // for the next pass
    if (pass == Pass::WaitForDependenciesGenerated)
    {
        VectorNative<AstNode*> done;
        while (!dependentNodesTmp.empty())
        {
            auto node = dependentNodesTmp.back();
            SWAG_ASSERT(node->extension && node->extension->bc);

            // Wait for the node if not generated
            {
                ScopedLock lk(node->mutex);
                if (!(node->semFlags & AST_SEM_BYTECODE_GENERATED))
                {
                    ScopedLock lk1(node->extension->byteCodeJob->mutexDependent);
                    waitingKind   = JobWaitKind::SemByteCodeGenerated;
                    waitingIdNode = node;
                    node->extension->byteCodeJob->dependentJobs.add(this);
                    return JobResult::KeepJobAlive;
                }
            }

            // Deal with registered dependent nodes, by adding them to the list
            dependentNodesTmp.pop_back();
            if (node == originalNode)
                continue;

            // Register the full dependency tree
            VectorNative<AstNode*> depNodes;
            getDependantCalls(node, depNodes);
            for (auto dep : depNodes)
            {
                ScopedLock lk(dep->mutex);
                if (!done.contains(dep))
                {
                    dependentNodesTmp.push_back_once(dep);
                    done.push_back(dep);
                }

                dependentNodes.push_back_once(dep);
            }
        }

        pass = Pass::ComputeDependenciesResolved;
    }

    // Inform dependencies that everything is done
    releaseByteCodeJob(originalNode);

    // Register function in compiler list, now that we are done
    if (originalNode->attributeFlags & ATTRIBUTE_COMPILER_FUNC)
        module->addCompilerFunc(originalNode->extension->bc);

    // #ast/#run etc... can have a #[Swag.printbc]. We need to print it now, because it's compile time, and the legit
    // pipeline for printing (after bc optimize) will not be called in that case
    if (originalNode->attributeFlags & ATTRIBUTE_PRINT_BC || (originalNode->ownerFct && originalNode->ownerFct->attributeFlags & ATTRIBUTE_PRINT_BC))
    {
        if (originalNode->attributeFlags & ATTRIBUTE_GENERATED_FUNC || originalNode->kind != AstNodeKind::FuncDecl)
            context.bc->print();
    }

    // Register runtime function type, by name
    if (sourceFile->isRuntimeFile && context.bc)
    {
        ScopedLock lk(sourceFile->module->mutexFile);
        SWAG_ASSERT(context.bc->typeInfoFunc);
        sourceFile->module->mapRuntimeFcts[context.bc->getCallName()] = context.bc;
    }

    if (context.bc &&
        context.bc->node &&
        context.bc->node->kind == AstNodeKind::FuncDecl)
    {
        auto funcNode = CastAst<AstFuncDecl>(context.bc->node, AstNodeKind::FuncDecl);
        if (!funcNode->sourceFile->numTestErrors)
        {
            // Be sure that every used registers have been released
            if (context.bc->maxReservedRegisterRC > context.bc->availableRegistersRC.size())
            {
                context.sourceFile->internalError(funcNode, Fmt("function `%s` does not release all registers !", funcNode->token.ctext()));
                if (originalNode->attributeFlags & ATTRIBUTE_PRINT_BC)
                    context.bc->print();
            }
            else if (context.bc->maxReservedRegisterRC < context.bc->availableRegistersRC.size())
            {
                context.sourceFile->internalError(funcNode, Fmt("function `%s` releases too many registers !", funcNode->token.ctext()));
                if (originalNode->attributeFlags & ATTRIBUTE_PRINT_BC)
                    context.bc->print();
            }
        }
    }

    return JobResult::ReleaseJob;
}