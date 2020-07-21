#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCode.h"
#include "Diagnostic.h"
#include "ThreadManager.h"
#include "SourceFile.h"
#include "ByteCodeOp.h"
#include "Module.h"
#include "Ast.h"
#include "Stats.h"
#include "TypeManager.h"
#include "Context.h"
#include "DiagnosticInfos.h"

thread_local Pool<ByteCodeGenJob> g_Pool_byteCodeGenJob;

bool ByteCodeGenJob::internalError(ByteCodeGenContext* context, const char* msg, AstNode* node)
{
    if (!node)
        node = context->node;
    context->sourceFile->report({node, format("internal error, %s", msg)});
    return false;
}

uint32_t ByteCodeGenJob::reserveRegisterRC(ByteCodeGenContext* context)
{
    if (!context->bc->availableRegistersRC.empty())
    {
        auto result = context->bc->availableRegistersRC.back();
        context->bc->availableRegistersRC.pop_back();
        return result;
    }

    return context->bc->maxReservedRegisterRC++;
}

void ByteCodeGenJob::reserveRegisterRC(ByteCodeGenContext* context, RegisterList& rc, int num)
{
    rc.clear();
    while (num--)
    {
        rc += reserveRegisterRC(context);
    }
}

void ByteCodeGenJob::reserveLinearRegisterRC(ByteCodeGenContext* context, RegisterList& rc, int num)
{
    freeRegisterRC(context, rc);

    // From linear cache
    if (num == 2 && !context->bc->availableRegistersRC2.empty())
    {
        auto r0 = context->bc->availableRegistersRC2.back();
        rc += r0;
        SWAG_ASSERT(r0 < context->bc->maxReservedRegisterRC);
        context->bc->availableRegistersRC2.pop_back();
        auto r1 = context->bc->availableRegistersRC2.back();
        rc += r1;
        SWAG_ASSERT(r1 < context->bc->maxReservedRegisterRC);
        context->bc->availableRegistersRC2.pop_back();
        return;
    }

    while (num--)
        rc += context->bc->maxReservedRegisterRC++;
}

void ByteCodeGenJob::truncRegisterRC(ByteCodeGenContext* context, RegisterList& rc, int count)
{
    if (rc.size() == count)
        return;

    RegisterList rs;

    for (int i = 0; i < count; i++)
        rs += rc[i];
    for (int i = count; i < rc.size(); i++)
        context->bc->availableRegistersRC.push_back(rc[i]);

    rc = rs;
}

void ByteCodeGenJob::freeRegisterRC(ByteCodeGenContext* context, RegisterList& rc)
{
    if (!rc.canFree)
        return;

    // Cache for linear
    if (rc.size() == 2 && rc[0] == rc[1] - 1)
    {
        SWAG_ASSERT(rc[1] < context->bc->maxReservedRegisterRC);
        context->bc->availableRegistersRC2.push_back(rc[1]);
        context->bc->availableRegistersRC2.push_back(rc[0]);
        rc.clear();
        return;
    }

    auto n = rc.size();
    for (int i = n - 1; i >= 0; i--)
    {
        freeRegisterRC(context, rc[i]);
    }

    rc.clear();
}

void ByteCodeGenJob::freeRegisterRC(ByteCodeGenContext* context, uint32_t rc)
{
#ifdef SWAG_HAS_ASSERT
    for (auto r : context->bc->availableRegistersRC)
        SWAG_ASSERT(r != rc);
#endif
    context->bc->availableRegistersRC.push_back(rc);
}

void ByteCodeGenJob::freeRegisterRC(ByteCodeGenContext* context, AstNode* node)
{
    if (!node)
        return;
    freeRegisterRC(context, node->resultRegisterRC);
    freeRegisterRC(context, node->additionalRegisterRC);
}

bool ByteCodeGenJob::emitPassThrough(ByteCodeGenContext* context)
{
    auto node              = context->node;
    node->resultRegisterRC = node->childs.back()->resultRegisterRC;
    return true;
}

ByteCodeInstruction* ByteCodeGenJob::emitInstruction(ByteCodeGenContext* context, ByteCodeOp op, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3)
{
    AstNode* node = context->node;
    auto     bc   = context->bc;

    if (bc->numInstructions == bc->maxInstructions)
    {
        bc->maxInstructions = bc->maxInstructions * 2;
        bc->maxInstructions = max(bc->maxInstructions, 32);
        auto newInstuctions = (ByteCodeInstruction*) malloc(bc->maxInstructions * sizeof(ByteCodeInstruction));
        memcpy(newInstuctions, bc->out, bc->numInstructions * sizeof(ByteCodeInstruction));
        free(bc->out);
        bc->out = newInstuctions;
    }

    SWAG_ASSERT(bc->out);
    ByteCodeInstruction& ins = bc->out[bc->numInstructions++];
    ins.op                   = op;
    ins.a.u64                = r0;
    ins.b.u64                = r1;
    ins.c.u64                = r2;
    ins.d.u64                = r3;
    ins.node                 = node;

    if (g_CommandLine.stats)
        g_Stats.numInstructions++;

    switch (op)
    {
    case ByteCodeOp::Jump:
    case ByteCodeOp::JumpIfTrue:
    case ByteCodeOp::JumpIfNotTrue:
    case ByteCodeOp::JumpIfNotZero32:
    case ByteCodeOp::JumpIfNotZero64:
    case ByteCodeOp::JumpIfZero32:
    case ByteCodeOp::JumpIfZero64:
        bc->numJumps++;
        break;
    }

    return &ins;
}

void ByteCodeGenJob::inherhitLocation(ByteCodeInstruction* inst, AstNode* node)
{
    inst->node = node;
}

void ByteCodeGenJob::askForByteCode(Job* dependentJob, Job* job, AstNode* node, uint32_t flags)
{
    if (!node)
        return;

    auto sourceFile = node->sourceFile;

    unique_lock lk(node->mutex);

    // If this is a foreign function, we do not need bytecode
    if (node->kind == AstNodeKind::FuncDecl)
    {
        auto funcDecl = CastAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
        if (funcDecl->attributeFlags & ATTRIBUTE_FOREIGN)
        {
            // Need to wait for function full semantic resolve
            if (flags & ASKBC_WAIT_SEMANTIC_RESOLVED)
            {
                if (!(funcDecl->flags & AST_FULL_RESOLVE))
                {
                    funcDecl->dependentJobs.add(job);
                    job->setPending(funcDecl->resolvedSymbolName);
                }
            }

            return;
        }
    }

    if (job)
    {
        // If true, then this is a simple recursive call
        if (node->byteCodeJob == job)
            return;

        // Need to wait for function full semantic resolve
        if (flags & ASKBC_WAIT_SEMANTIC_RESOLVED)
        {
            if (!(node->flags & AST_FULL_RESOLVE))
            {
                auto funcDecl = CastAst<AstFuncDecl>(node, AstNodeKind::FuncDecl);
                funcDecl->dependentJobs.add(job);
                job->setPending(funcDecl->resolvedSymbolName);
                return;
            }
        }
    }

    // Need to generate bytecode, if not already done or running
    if (!(node->flags & AST_BYTECODE_GENERATED))
    {
        if (flags & ASKBC_ADD_DEP_NODE)
        {
            SWAG_ASSERT(job);
            job->dependentNodes.push_back(node);
        }

        if (flags & ASKBC_WAIT_DONE)
        {
            SWAG_ASSERT(job);
            job->setPending(nullptr);
        }

        if (!node->byteCodeJob)
        {
            node->byteCodeJob               = g_Pool_byteCodeGenJob.alloc();
            node->byteCodeJob->sourceFile   = sourceFile;
            node->byteCodeJob->module       = sourceFile->module;
            node->byteCodeJob->dependentJob = dependentJob;
            node->byteCodeJob->nodes.push_back(node);
            if (flags & ASKBC_WAIT_DONE)
            {
                SWAG_ASSERT(job);
                node->byteCodeJob->dependentJobs.add(job);
            }

            node->bc             = g_Allocator.alloc<ByteCode>();
            node->bc->node       = node;
            node->bc->sourceFile = node->sourceFile;
            node->bc->name       = node->ownerScope->fullname + "_" + node->name.c_str();
            node->bc->name.replaceAll('.', '_');
            if (node->kind == AstNodeKind::FuncDecl)
                sourceFile->module->addByteCodeFunc(node->bc);

            g_ThreadMgr.addJob(node->byteCodeJob);
        }

        return;
    }

    if (flags & ASKBC_WAIT_RESOLVED)
    {
        SWAG_ASSERT(job);
        if (!(node->flags & AST_BYTECODE_RESOLVED) && node->byteCodeJob)
        {
            node->byteCodeJob->dependentJobs.add(job);
            job->setPending(nullptr);
            return;
        }
    }
}

JobResult ByteCodeGenJob::execute()
{
    scoped_lock lkExecute(executeMutex);

    if (!originalNode)
    {
        SWAG_ASSERT(nodes.size() == 1);
        originalNode = nodes.front();
    }

#ifdef SWAG_HAS_ASSERT
    PushDiagnosticInfos di;
    if (g_CommandLine.devMode)
    {
        g_diagnosticInfos.last().message    = "ByteCodeGenJob";
        g_diagnosticInfos.last().sourceFile = sourceFile;
        g_diagnosticInfos.last().node       = originalNode;
    }
#endif

    baseContext        = &context;
    context.job        = this;
    context.sourceFile = sourceFile;
    context.bc         = originalNode->bc;
    context.node       = originalNode;

    if (!syncToDependentNodes)
    {
        // Register SystemAllocator interface to the default bytecode context
        if (sourceFile->swagFile && (originalNode->name == "SystemAllocator"))
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
            g_defaultContext.allocator.data   = nullptr;
            g_defaultContext.allocator.itable = (void*) itable;
        }

        while (!nodes.empty())
        {
            auto node = nodes.back();

#ifdef SWAG_HAS_ASSERT
            if (g_CommandLine.devMode)
            {
                g_diagnosticInfos.last().node = node;
            }
#endif

            context.node   = node;
            context.result = ContextResult::Done;

            switch (node->bytecodeState)
            {
            case AstNodeResolveState::Enter:
                node->bytecodeState = AstNodeResolveState::ProcessingChilds;

                if (node->byteCodeBeforeFct && !node->byteCodeBeforeFct(&context))
                    return JobResult::ReleaseJob;
                SWAG_ASSERT(context.result == ContextResult::Done);

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
                            return JobResult::ReleaseJob;
                        if (!emitCast(&context, node, TypeManager::concreteType(node->typeInfo), node->castedTypeInfo))
                            return JobResult::ReleaseJob;
                        // To be sure that cast is treated once
                        node->castedTypeInfo = nullptr;
                        SWAG_ASSERT(context.result == ContextResult::Done);
                    }
                    else if (node->byteCodeFct)
                    {
                        if (!node->byteCodeFct(&context))
                            return JobResult::ReleaseJob;
                        if (context.result == ContextResult::Pending)
                            return JobResult::KeepJobAlive;
                        if (context.result == ContextResult::NewChilds)
                            continue;
                    }

                    node->bytecodeState = AstNodeResolveState::PostChilds;
                }

            case AstNodeResolveState::PostChilds:
                if (node->byteCodeAfterFct)
                {
                    if (!node->byteCodeAfterFct(&context))
                        return JobResult::ReleaseJob;
                    if (context.result == ContextResult::Pending)
                        return JobResult::KeepJobAlive;
                    if (context.result == ContextResult::NewChilds)
                        continue;
                }

                nodes.pop_back();
                break;
            }
        }

        if (context.bc)
            emitInstruction(&context, ByteCodeOp::End);

        // Print resulting bytecode
        if (originalNode->attributeFlags & ATTRIBUTE_PRINTBYTECODE)
            context.bc->print();
    }

    // Inform dependencies that this node has bytecode
    {
        unique_lock lk(originalNode->mutex);
        originalNode->flags |= AST_BYTECODE_GENERATED;
        dependentJobs.setRunning();
    }

    // Wait for other dependent nodes to be generated
    syncToDependentNodes = true;
    while (!dependentNodes.empty())
    {
        auto        node = dependentNodes.back();
        unique_lock lk(node->mutex);
        if (node->flags & AST_BYTECODE_GENERATED)
        {
            dependentNodes.pop_back();
            continue;
        }

        node->byteCodeJob->dependentJobs.add(this);
        return JobResult::KeepJobAlive;
    }

    // Inform dependencies that everything is done
    {
        unique_lock lk(originalNode->mutex);
        originalNode->flags |= AST_BYTECODE_RESOLVED;
        originalNode->byteCodeJob = nullptr;
    }

    return JobResult::ReleaseJob;
}
