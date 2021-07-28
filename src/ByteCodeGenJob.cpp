#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCode.h"
#include "Diagnostic.h"
#include "ThreadManager.h"
#include "Module.h"
#include "Ast.h"
#include "TypeManager.h"
#include "Context.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"

bool ByteCodeGenJob::internalError(ByteCodeGenContext* context, const char* msg, AstNode* node)
{
    if (!node)
        node = context->node;
    context->sourceFile->report({node, Utf8::format(Msg0058, msg)});
    return false;
}

void ByteCodeGenJob::reserveRegisterRC(ByteCodeGenContext* context, RegisterList& rc, int num)
{
    rc.clear();
    rc.canFree = true;
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
    sort(context->bc->availableRegistersRC.begin(), context->bc->availableRegistersRC.end(), [](uint32_t a, uint32_t b) { return a > b; });
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
        SWAG_ASSERT(resultRegisterRC.canFree);
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

    SWAG_ASSERT(rc.canFree);

    RegisterList rs;
    for (int i = 0; i < count; i++)
        rs += rc[i];

    if (rc.canFree)
    {
        for (int i = count; i < rc.size(); i++)
            freeRegisterRC(context, rc[i]);
    }

    rs.canFree = rc.canFree;
    rc         = rs;
}

void ByteCodeGenJob::freeRegisterRC(ByteCodeGenContext* context, RegisterList& rc)
{
    if (!rc.canFree)
        return;
    for (int i = 0; i < rc.size(); i++)
        freeRegisterRC(context, rc[i]);
    rc.clear();
    rc.canFree = true;
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
    if (!r0.canFree)
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

void ByteCodeGenJob::emitDebugLine(ByteCodeGenContext* context, AstNode* node)
{
    if (!context->sourceFile->module->buildCfg.byteCodeOptimize)
    {
        PushLocation lk(context, &node->token.endLocation);
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

ByteCodeInstruction* ByteCodeGenJob::emitMakeSegPointer(ByteCodeGenContext* context, DataSegment* seg, uint32_t r0, uint32_t offset)
{
    SWAG_ASSERT(seg);
    switch (seg->kind)
    {
    case SegmentKind::Data:
        return emitInstruction(context, ByteCodeOp::MakeMutableSegPointer, r0, offset);
    case SegmentKind::Bss:
        return emitInstruction(context, ByteCodeOp::MakeBssSegPointer, r0, offset);
    case SegmentKind::Compiler:
        return emitInstruction(context, ByteCodeOp::MakeCompilerSegPointer, r0, offset);
    case SegmentKind::Constant:
        return emitInstruction(context, ByteCodeOp::MakeConstantSegPointer, r0, offset);
    default:
        SWAG_ASSERT(false);
    }

    return nullptr;
}

ByteCodeInstruction* ByteCodeGenJob::emitGetFromSeg(ByteCodeGenContext* context, DataSegment* seg, uint32_t r0, uint32_t offset)
{
    SWAG_ASSERT(seg);
    switch (seg->kind)
    {
    case SegmentKind::Data:
        return emitInstruction(context, ByteCodeOp::GetFromMutableSeg64, r0, offset);
    case SegmentKind::Bss:
        return emitInstruction(context, ByteCodeOp::GetFromBssSeg64, r0, offset);
    case SegmentKind::Compiler:
        return emitInstruction(context, ByteCodeOp::GetFromCompilerSeg64, r0, offset);
    default:
        SWAG_ASSERT(false);
    }

    return nullptr;
}

ByteCodeInstruction* ByteCodeGenJob::emitInstruction(ByteCodeGenContext* context, ByteCodeOp op, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3)
{
    AstNode* node = context->node;
    auto     bc   = context->bc;
    SWAG_ASSERT(!bc->isRunning);

    if (bc->numInstructions == bc->maxInstructions)
    {
        auto oldSize        = (int) (bc->maxInstructions * sizeof(ByteCodeInstruction));
        bc->maxInstructions = bc->maxInstructions * 2;
        bc->maxInstructions = max(bc->maxInstructions, 8);
        auto newInstuctions = (ByteCodeInstruction*) g_Allocator.alloc(bc->maxInstructions * sizeof(ByteCodeInstruction));
        memcpy(newInstuctions, bc->out, bc->numInstructions * sizeof(ByteCodeInstruction));
        g_Allocator.free(bc->out, oldSize);
        if (g_CommandLine.stats)
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

    if (context->noLocation)
        ins.location = nullptr;
    else if (context->forceLocation)
        ins.location = context->forceLocation;
    else
        ins.location = &node->token.startLocation;

    if (ByteCode::isJump(&ins))
        bc->numJumps++;

    switch (op)
    {
    case ByteCodeOp::IntrinsicS8x1:
    case ByteCodeOp::IntrinsicS16x1:
    case ByteCodeOp::IntrinsicS32x1:
    case ByteCodeOp::IntrinsicS64x1:
    case ByteCodeOp::IntrinsicF32x1:
    case ByteCodeOp::IntrinsicF64x1:
    case ByteCodeOp::IntrinsicCStrLen:
        context->bc->maxCallParams = max(context->bc->maxCallParams, 2); // Runtime call
        break;

    case ByteCodeOp::IntrinsicS8x2:
    case ByteCodeOp::IntrinsicS16x2:
    case ByteCodeOp::IntrinsicS32x2:
    case ByteCodeOp::IntrinsicS64x2:
    case ByteCodeOp::IntrinsicU8x2:
    case ByteCodeOp::IntrinsicU16x2:
    case ByteCodeOp::IntrinsicU32x2:
    case ByteCodeOp::IntrinsicU64x2:
    case ByteCodeOp::IntrinsicF32x2:
    case ByteCodeOp::IntrinsicF64x2:
    case ByteCodeOp::IntrinsicInterfaceOf:
    case ByteCodeOp::IntrinsicErrorMsg:
    case ByteCodeOp::IntrinsicPanic:
    case ByteCodeOp::IntrinsicAtomicAddS8:
    case ByteCodeOp::IntrinsicAtomicAddS16:
    case ByteCodeOp::IntrinsicAtomicAddS32:
    case ByteCodeOp::IntrinsicAtomicAddS64:
    case ByteCodeOp::IntrinsicAtomicAndS8:
    case ByteCodeOp::IntrinsicAtomicAndS16:
    case ByteCodeOp::IntrinsicAtomicAndS32:
    case ByteCodeOp::IntrinsicAtomicAndS64:
    case ByteCodeOp::IntrinsicAtomicOrS8:
    case ByteCodeOp::IntrinsicAtomicOrS16:
    case ByteCodeOp::IntrinsicAtomicOrS32:
    case ByteCodeOp::IntrinsicAtomicOrS64:
    case ByteCodeOp::IntrinsicAtomicXorS8:
    case ByteCodeOp::IntrinsicAtomicXorS16:
    case ByteCodeOp::IntrinsicAtomicXorS32:
    case ByteCodeOp::IntrinsicAtomicXorS64:
        context->bc->maxCallParams = max(context->bc->maxCallParams, 3); // Runtime call
        break;

    case ByteCodeOp::IntrinsicTypeCmp:
    case ByteCodeOp::IntrinsicMemCmp:
    case ByteCodeOp::IntrinsicAtomicCmpXchgS8:
    case ByteCodeOp::IntrinsicAtomicCmpXchgS16:
    case ByteCodeOp::IntrinsicAtomicCmpXchgS32:
    case ByteCodeOp::IntrinsicAtomicCmpXchgS64:
        context->bc->maxCallParams = max(context->bc->maxCallParams, 4); // Runtime call
        break;
    case ByteCodeOp::InternalPanic:
    case ByteCodeOp::IntrinsicStrCmp:
        context->bc->maxCallParams = max(context->bc->maxCallParams, 5); // Runtime call
        break;
    }

    return &ins;
}

void ByteCodeGenJob::inherhitLocation(ByteCodeInstruction* inst, AstNode* node)
{
    inst->node = node;
}

void ByteCodeGenJob::askForByteCode(Job* job, AstNode* node, uint32_t flags)
{
    if (!node)
        return;

    unique_lock lk(node->mutex);
    auto        sourceFile = node->sourceFile;

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
                if (!(funcDecl->semFlags & AST_SEM_FULL_RESOLVE))
                {
                    funcDecl->dependentJobs.add(job);
                    job->setPending(funcDecl->resolvedSymbolName, "AST_SEM_FULL_RESOLVE", funcDecl, nullptr);
                }
            }

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
            if (!(node->semFlags & AST_SEM_FULL_RESOLVE))
            {
                SWAG_ASSERT(funcDecl);
                funcDecl->dependentJobs.add(job);
                job->setPending(funcDecl->resolvedSymbolName, "AST_SEM_FULL_RESOLVE", funcDecl, nullptr);
                return;
            }
        }
    }

    // Register the node dependency
    if (flags & ASKBC_ADD_DEP_NODE)
    {
        SWAG_ASSERT(job);
        scoped_lock lk1(job->mutexDependent);
        job->dependentNodes.push_back(node);
    }

    // Need to generate bytecode, if not already done or running
    if (!(node->semFlags & AST_SEM_BYTECODE_GENERATED))
    {
        if (flags & ASKBC_WAIT_DONE)
        {
            SWAG_ASSERT(job);
            job->setPending(nullptr, "ASKBC_WAIT_DONE", node, nullptr);
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
            extension->byteCodeJob->context.expansionNode = job->baseContext->expansionNode;
            extension->byteCodeJob->nodes.push_back(node);
            extension->bc               = g_Allocator.alloc<ByteCode>();
            extension->bc->node         = node;
            extension->bc->sourceFile   = node->sourceFile;
            extension->bc->typeInfoFunc = funcDecl ? (TypeInfoFuncAttr*) funcDecl->typeInfo : nullptr;
            if (node->flags & AST_DEFINED_INTRINSIC)
                extension->bc->name = node->token.text;
            else if (node->sourceFile->isRuntimeFile)
                extension->bc->name = node->token.text.c_str();
            else
                extension->bc->name = node->ownerScope->getFullName() + "_" + node->token.text.c_str();
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
            scoped_lock lk1(extension->byteCodeJob->mutexDependent);
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
            job->setPending(nullptr, "ASKBC_WAIT_RESOLVED", node, nullptr);
            return;
        }
    }
}

void ByteCodeGenJob::releaseByteCodeJob(AstNode* node)
{
    unique_lock lk(node->mutex);
    node->semFlags |= AST_SEM_BYTECODE_RESOLVED | AST_SEM_BYTECODE_GENERATED;
    node->extension->byteCodeJob = nullptr;
}

JobResult ByteCodeGenJob::execute()
{
    scoped_lock lkExecute(executeMutex);

    if (!originalNode)
    {
        SWAG_ASSERT(nodes.size() == 1);
        originalNode = nodes.front();
    }

    SWAG_ASSERT(originalNode->extension);
    SWAG_ASSERT(originalNode->extension->byteCodeJob);

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
        // Register SystemAllocator interface to the default bytecode context
        if (sourceFile->isRuntimeFile && (originalNode->token.text == g_LangSpec.name_SystemAllocator))
        {
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(originalNode->typeInfo, TypeInfoKind::Struct);
            context.result  = ContextResult::Done;
            waitForAllStructInterfaces(typeStruct);
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

            auto node      = nodes.back();
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
                if (g_CommandLine.stats)
                    g_Stats.numInstructions += context.bc->numInstructions;

                // Print resulting bytecode
                if (originalNode->attributeFlags & ATTRIBUTE_PRINT_BC)
                {
                    unique_lock lk(module->mutexByteCode);
                    module->byteCodePrintBC.push_back(context.bc);
                }
            }
        }

        // Byte code is generated (but not yet resolved, as we need all dependencies to be resolved too)
        {
            unique_lock lk(originalNode->mutex);
            SWAG_ASSERT(originalNode->extension && originalNode->extension->byteCodeJob);
            originalNode->semFlags |= AST_SEM_BYTECODE_GENERATED;
            dependentJobs.setRunning();
        }

        pass              = Pass::WaitForDependenciesGenerated;
        dependentNodesTmp = dependentNodes;
    }

    // Wait for other dependent nodes to be generated
    // That way we are sure that every one has registered depend nodes, so the full dependency graph is completed
    // for the new pass
    if (pass == Pass::WaitForDependenciesGenerated)
    {
        while (!dependentNodesTmp.empty())
        {
            auto        node = dependentNodesTmp.back();
            unique_lock lk(node->mutex);
            if (node->semFlags & AST_SEM_BYTECODE_GENERATED)
            {
                dependentNodesTmp.pop_back();
                continue;
            }

            scoped_lock lk1(node->extension->byteCodeJob->mutexDependent);
            node->extension->byteCodeJob->dependentJobs.add(this);
            return JobResult::KeepJobAlive;
        }

        pass = Pass::ComputeDependenciesResolved;
    }

    // Determine if we have a loop (a dependent node depends on this job too)
    // If this is the case, then we eliminate that dependency
    if (pass == Pass::ComputeDependenciesResolved)
    {
        dependentNodesTmp.clear();
        for (int i = 0; i < dependentNodes.size(); i++)
        {
            auto node = dependentNodes[i];

            // If the node is already solved, remove it from the list
            {
                unique_lock lk(node->mutex);
                if (node->semFlags & AST_SEM_BYTECODE_RESOLVED)
                    continue;
            }

            // Compute the full dependency list
            bool mustWait = true;

            set<AstNode*>          done;
            VectorNative<AstNode*> tmp;
            tmp.push_back(node);
            for (int toScan = 0; toScan < tmp.size(); toScan++)
            {
                auto dep = tmp[toScan];

                unique_lock lkDep(dep->mutex);
                if (!dep->extension || !dep->extension->byteCodeJob)
                    continue;

                if (dep->extension->byteCodeJob == this)
                {
                    mustWait = false;
                    break;
                }

                if (dep->extension->byteCodeJob->dependentNodes.empty())
                    continue;

                shared_lock lk1(dep->extension->byteCodeJob->mutexDependent);
                for (auto newDep : dep->extension->byteCodeJob->dependentNodes)
                {
                    auto it = done.find(newDep);
                    if (it != done.end())
                        continue;
                    done.insert(newDep);
                    tmp.push_back(newDep);
                }
            }

            if (mustWait)
                dependentNodesTmp.push_back(node);
        }

        pass = Pass::WaitForDependenciesResolved;
    }

    // Wait for other dependent nodes to be resolved
    if (pass == Pass::WaitForDependenciesResolved)
    {
        while (!dependentNodesTmp.empty())
        {
            auto node = dependentNodesTmp.back();

            unique_lock lk(node->mutex);
            if (node->semFlags & AST_SEM_BYTECODE_RESOLVED)
            {
                dependentNodesTmp.pop_back();
                continue;
            }

            scoped_lock lk1(node->extension->byteCodeJob->mutexDependent);
            node->extension->byteCodeJob->dependentJobs.add(this);
            return JobResult::KeepJobAlive;
        }
    }

    // Inform dependencies that everything is done
    releaseByteCodeJob(originalNode);

    // Register function in compiler list, now that we are done
    if (originalNode->attributeFlags & ATTRIBUTE_COMPILER_FUNC)
        module->addCompilerFunc(originalNode->extension->bc);

    // #ast can have a #[Swag.printbc]. We need to print it now, because it's compile time, and the legit
    // pipeline for printing (after bc optimize) will not be called in that case
    if (originalNode->attributeFlags & ATTRIBUTE_PRINT_BC)
    {
        if (originalNode->attributeFlags & ATTRIBUTE_AST_FUNC || originalNode->attributeFlags & ATTRIBUTE_COMPILER_FUNC)
            context.bc->print();
    }

    // Register runtime function type, by name
    if (sourceFile->isRuntimeFile && context.bc)
    {
        unique_lock lk(sourceFile->module->mutexFile);
        SWAG_ASSERT(context.bc->typeInfoFunc);
        sourceFile->module->mapRuntimeFcts[context.bc->callName()] = context.bc;
    }

    // Be sure that every used registers have been released
    if (context.bc &&
        context.bc->node &&
        context.bc->node->kind == AstNodeKind::FuncDecl &&
        !context.bc->node->sourceFile->numTestErrors)
    {
        if (context.bc->maxReservedRegisterRC > context.bc->availableRegistersRC.size())
            context.sourceFile->report({context.bc->node, context.bc->node->token, Utf8::format(Msg0059, context.bc->node->token.text.c_str()), DiagnosticLevel::Warning});
        else if (context.bc->maxReservedRegisterRC < context.bc->availableRegistersRC.size())
        {
            context.sourceFile->report({context.bc->node, context.bc->node->token, Utf8::format(Msg0060, context.bc->node->token.text.c_str())});
            if (originalNode->attributeFlags & ATTRIBUTE_PRINT_BC)
                context.bc->print();
        }
    }

    return JobResult::ReleaseJob;
}