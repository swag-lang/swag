#include "pch.h"
#include "ByteCodeGen.h"
#include "Ast.h"
#include "ByteCode.h"
#include "ByteCodeGenJob.h"
#include "Context.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Semantic.h"
#include "ThreadManager.h"
#include "TypeManager.h"

bool ByteCodeGen::setupRuntime(ByteCodeGenContext* context, AstNode* node)
{
    // Register allocator interface to the default bytecode context
    if (node->token.text == g_LangSpec->name_SystemAllocator)
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
        Semantic::waitAllStructInterfaces(context->baseJob, typeStruct);
        YIELD();
        SWAG_ASSERT(typeStruct->interfaces.size() == 1);
        auto itable = context->sourceFile->module->constantSegment.address(typeStruct->interfaces[0]->offset);
        SWAG_ASSERT(itable);
        SWAG_ASSERT(((void**) itable)[0]);
        g_SystemAllocatorTable = itable;
    }

    if (node->token.text == g_LangSpec->name_DebugAllocator)
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
        Semantic::waitAllStructInterfaces(context->baseJob, typeStruct);
        YIELD();
        SWAG_ASSERT(typeStruct->interfaces.size() == 1);
        auto itable = context->sourceFile->module->constantSegment.address(typeStruct->interfaces[0]->offset);
        SWAG_ASSERT(itable);
        SWAG_ASSERT(((void**) itable)[0]);
        g_DebugAllocatorTable = itable;
    }

    if (g_SystemAllocatorTable && g_DebugAllocatorTable)
    {
        g_DefaultContext.allocator.data   = nullptr;
        g_DefaultContext.allocator.itable = g_SystemAllocatorTable;
    }

    return true;
}

bool ByteCodeGen::emitPassThrough(ByteCodeGenContext* context)
{
    auto node = context->node;
    if (node->childs.empty())
        return true;

    auto child = node->childs.back();
    if (node->flags & AST_DISCARD)
        freeRegisterRC(context, child);
    else
    {
        SWAG_CHECK(emitCast(context, child, TypeManager::concreteType(child->typeInfo), child->castedTypeInfo));
        node->resultRegisterRC = child->resultRegisterRC;
    }

    if (child->hasExtMisc())
        freeRegisterRC(context, child->extMisc()->additionalRegisterRC);

    return true;
}

bool ByteCodeGen::skipNodes(ByteCodeGenContext* context, AstNode* node)
{
    node->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;
    auto res = Ast::visit(context, node, [](ErrorContext* cxt, AstNode* n)
                          {
                              if (n->kind != AstNodeKind::Literal)
                                  return Ast::VisitResult::Continue;
                              if (n->semFlags & SEMFLAG_LITERAL_SUFFIX)
                              {
                                  cxt->report({ n->childs.front(), Fmt(Err(Err0532), n->childs.front()->token.ctext()) });
                                  return Ast::VisitResult::Stop;
                              }

                              return Ast::VisitResult::Continue; });

    return res == Ast::VisitResult::Stop ? false : true;
}

bool ByteCodeGen::emitUnreachable(ByteCodeGenContext* context)
{
    SWAG_CHECK(emitSafetyUnreachable(context));
    return true;
}

void ByteCodeGen::emitDebugLine(ByteCodeGenContext* context, AstNode* node)
{
    if (!node)
        return;
    PushLocation lk(context, &node->token.endLocation);
    emitDebugLine(context);
}

void ByteCodeGen::emitDebugLine(ByteCodeGenContext* context)
{
    if (context->sourceFile->module->buildCfg.backendDebugInformations &&
        context->sourceFile->module->buildCfg.byteCodeOptimizeLevel == 0 &&
        !context->sourceFile->isScriptFile)
    {
        EMIT_INST0(context, ByteCodeOp::DebugNop);
    }
}

bool ByteCodeGen::emitDebugNop(ByteCodeGenContext* context)
{
    auto node = context->node;
    if (!node->childs.empty())
        node->resultRegisterRC = node->childs.back()->resultRegisterRC;
    emitDebugLine(context, node);
    return true;
}

ByteCodeInstruction* ByteCodeGen::emitMakeSegPointer(ByteCodeGenContext* context, DataSegment* storageSegment, uint32_t storageOffset, uint32_t r0)
{
    SWAG_ASSERT(storageSegment);
    switch (storageSegment->kind)
    {
    case SegmentKind::Data:
        return EMIT_INST2(context, ByteCodeOp::MakeMutableSegPointer, r0, storageOffset);
    case SegmentKind::Bss:
        return EMIT_INST2(context, ByteCodeOp::MakeBssSegPointer, r0, storageOffset);
    case SegmentKind::Compiler:
        return EMIT_INST2(context, ByteCodeOp::MakeCompilerSegPointer, r0, storageOffset);
    case SegmentKind::Constant:
        return EMIT_INST2(context, ByteCodeOp::MakeConstantSegPointer, r0, storageOffset);
    default:
        SWAG_ASSERT(false);
    }

    return nullptr;
}

ByteCodeInstruction* ByteCodeGen::emitGetFromSeg(ByteCodeGenContext* context, DataSegment* storageSegment, uint32_t storageOffset, uint32_t r0, uint32_t numBits)
{
    SWAG_ASSERT(storageSegment);

    if (numBits == 8)
    {
        switch (storageSegment->kind)
        {
        case SegmentKind::Data:
            return EMIT_INST2(context, ByteCodeOp::GetFromMutableSeg8, r0, storageOffset);
        case SegmentKind::Bss:
            return EMIT_INST2(context, ByteCodeOp::GetFromBssSeg8, r0, storageOffset);
        case SegmentKind::Compiler:
            return EMIT_INST2(context, ByteCodeOp::GetFromCompilerSeg8, r0, storageOffset);
        default:
            SWAG_ASSERT(false);
        }
    }
    else if (numBits == 16)
    {
        switch (storageSegment->kind)
        {
        case SegmentKind::Data:
            return EMIT_INST2(context, ByteCodeOp::GetFromMutableSeg16, r0, storageOffset);
        case SegmentKind::Bss:
            return EMIT_INST2(context, ByteCodeOp::GetFromBssSeg16, r0, storageOffset);
        case SegmentKind::Compiler:
            return EMIT_INST2(context, ByteCodeOp::GetFromCompilerSeg16, r0, storageOffset);
        default:
            SWAG_ASSERT(false);
        }
    }
    else if (numBits == 32)
    {
        switch (storageSegment->kind)
        {
        case SegmentKind::Data:
            return EMIT_INST2(context, ByteCodeOp::GetFromMutableSeg32, r0, storageOffset);
        case SegmentKind::Bss:
            return EMIT_INST2(context, ByteCodeOp::GetFromBssSeg32, r0, storageOffset);
        case SegmentKind::Compiler:
            return EMIT_INST2(context, ByteCodeOp::GetFromCompilerSeg32, r0, storageOffset);
        default:
            SWAG_ASSERT(false);
        }
    }
    else if (numBits == 64)
    {
        switch (storageSegment->kind)
        {
        case SegmentKind::Data:
            return EMIT_INST2(context, ByteCodeOp::GetFromMutableSeg64, r0, storageOffset);
        case SegmentKind::Bss:
            return EMIT_INST2(context, ByteCodeOp::GetFromBssSeg64, r0, storageOffset);
        case SegmentKind::Compiler:
            return EMIT_INST2(context, ByteCodeOp::GetFromCompilerSeg64, r0, storageOffset);
        default:
            SWAG_ASSERT(false);
        }
    }
    else
    {
        SWAG_ASSERT(false);
    }

    return nullptr;
}

ByteCodeInstruction* ByteCodeGen::emitInstruction(ByteCodeGenContext* context, ByteCodeOp op, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3, const char* file, uint32_t line)
{
    AstNode* node = context->node;
    auto     bc   = context->bc;

    context->bc->makeRoomForInstructions();

    SWAG_RACE_CONDITION_WRITE(bc->raceCond);
    SWAG_ASSERT(bc->out);
    ByteCodeInstruction& ins = bc->out[bc->numInstructions++];

    ins.op = op;

    ins.a.u64 = r0;
    ins.b.u64 = r1;
    ins.c.u64 = r2;
    ins.d.u64 = r3;

    ins.flags    = context->instructionsFlags;
    ins.dynFlags = 0;

    ins.node = context->forceNode ? context->forceNode : node;

    if (context->tryCatchScope)
        ins.flags |= BCI_TRYCATCH;
    if (ins.node && ins.node->attributeFlags & ATTRIBUTE_CAN_OVERFLOW_ON)
        ins.flags |= BCI_CAN_OVERFLOW;

#if defined SWAG_DEV_MODE
    ins.sourceFile            = file;
    ins.sourceLine            = line;
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
    // 5 is the maximum registers needed for a given op call (4 registers + 1 return).
    context->bc->maxCallParams = max(context->bc->maxCallParams, 5);
    return &ins;
}

void ByteCodeGen::inherhitLocation(ByteCodeInstruction* inst, AstNode* node)
{
    inst->node = node;
}

void ByteCodeGen::askForByteCode(Job* job, AstNode* node, uint32_t flags, ByteCode* caller)
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

        if (funcDecl->isForeign())
        {
            // Need to wait for function full semantic resolve
            if (flags & ASKBC_WAIT_SEMANTIC_RESOLVED)
            {
                SWAG_ASSERT(job);
                Semantic::waitFuncDeclFullResolve(job, funcDecl);
                if (job->baseContext->result != ContextResult::Done)
                    return;
            }

            ScopedLock lk(node->mutex);
            node->semFlags |= SEMFLAG_BYTECODE_GENERATED | SEMFLAG_BYTECODE_RESOLVED;
            return;
        }
    }

    if (job)
    {
        // If true, then this is a simple recursive call
        if (node->hasExtByteCode() && node->extByteCode()->byteCodeJob == job)
            return;

        // Need to wait for function full semantic resolve
        if (flags & ASKBC_WAIT_SEMANTIC_RESOLVED)
        {
            SWAG_ASSERT(funcDecl);
            Semantic::waitFuncDeclFullResolve(job, funcDecl);
            if (job->baseContext->result != ContextResult::Done)
                return;
        }
    }

    if (caller)
        caller->dependentCalls.push_back_once(node);

    ScopedLock lk(node->mutex);

    // Need to generate bytecode, if not already done or running
    if (!(node->semFlags & SEMFLAG_BYTECODE_GENERATED))
    {
        if (flags & ASKBC_WAIT_DONE)
        {
            SWAG_ASSERT(job);
            job->setPending(JobWaitKind::SemByteCodeGenerated, nullptr, node, nullptr);
        }

        node->allocateExtensionNoLock(ExtensionKind::ByteCode);
        auto extension = node->extByteCode();
        if (!extension->byteCodeJob)
        {
            Job* dependentJob;
            if (flags & ASKBC_WAIT_DONE)
                dependentJob = job;
            else
                dependentJob = job->dependentJob;
            extension->byteCodeJob                      = ByteCodeGenJob::newJob(dependentJob, sourceFile, node);
            extension->byteCodeJob->context.errCxtSteps = job->baseContext->errCxtSteps;
            if (!extension->bc)
            {
                extension->bc             = Allocator::alloc<ByteCode>();
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
                sourceFile->module->addByteCodeFunc(node->extByteCode()->bc);

            if (flags & ASKBC_WAIT_DONE)
                job->jobsToAdd.push_back(node->extByteCode()->byteCodeJob);
            else
                g_ThreadMgr.addJob(node->extByteCode()->byteCodeJob);
        }
        else if (flags & ASKBC_WAIT_DONE)
        {
            ScopedLock lk1(extension->byteCodeJob->mutexDependent);
            extension->byteCodeJob->dependentJobs.add(job);
        }

        return;
    }

    if (flags & ASKBC_WAIT_RESOLVED)
    {
        SWAG_ASSERT(job);
        if (!(node->semFlags & SEMFLAG_BYTECODE_RESOLVED))
        {
            auto extension = node->extByteCode();
            SWAG_ASSERT(extension && extension->byteCodeJob);

            ScopedLock lk1(extension->byteCodeJob->mutexDependent);
            job->setPending(JobWaitKind::AskBcWaitResolve, nullptr, node, nullptr);
            extension->byteCodeJob->dependentJobs.add(job);
            return;
        }
    }
}

void ByteCodeGen::releaseByteCodeJob(AstNode* node)
{
    ScopedLock lk(node->mutex);
    node->semFlags |= SEMFLAG_BYTECODE_RESOLVED | SEMFLAG_BYTECODE_GENERATED;
    SWAG_ASSERT(node->hasExtByteCode());
    node->extByteCode()->byteCodeJob = nullptr;
}

void ByteCodeGen::getDependantCalls(AstNode* depNode, VectorNative<AstNode*>& dep)
{
    // Normal function
    SWAG_ASSERT(depNode->hasExtByteCode());
    if (depNode->extByteCode()->bc)
    {
        dep.append(depNode->extByteCode()->bc->dependentCalls);
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