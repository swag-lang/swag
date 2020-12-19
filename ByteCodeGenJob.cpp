#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCode.h"
#include "Diagnostic.h"
#include "ThreadManager.h"
#include "ByteCodeOp.h"
#include "Module.h"
#include "Ast.h"
#include "TypeManager.h"
#include "Context.h"

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
    // From the normal cache
    if (!context->bc->availableRegistersRC.empty())
    {
        auto result = context->bc->availableRegistersRC.back();
        context->bc->availableRegistersRC.pop_back();
        if (result < MAX_CACHE_FREE_REG)
        {
            SWAG_ASSERT(context->bc->regIsFree[result] != UINT32_MAX);
            context->bc->regIsFree[result] = UINT32_MAX;
        }
        return result;
    }

    // From the linear cache
    if (!context->bc->availableRegistersRC2.empty())
    {
        auto r0 = context->bc->availableRegistersRC2.back();
        context->bc->availableRegistersRC2.pop_back();
        auto r1 = context->bc->availableRegistersRC2.back();
        context->bc->availableRegistersRC2.pop_back();
        if (r1 < MAX_CACHE_FREE_REG)
        {
            SWAG_ASSERT(context->bc->regIsFree[r1] == UINT32_MAX);
            context->bc->regIsFree[r1] = (uint32_t) context->bc->availableRegistersRC.size();
        }
        context->bc->availableRegistersRC.push_back(r1);

        return r0;
    }

    return context->bc->maxReservedRegisterRC++;
}

void ByteCodeGenJob::reserveRegisterRC(ByteCodeGenContext* context, RegisterList& rc, int num)
{
    rc.clear();
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

void ByteCodeGenJob::reserveLinearRegisterRC2(ByteCodeGenContext* context, RegisterList& rc)
{
    freeRegisterRC(context, rc);

    // From linear cache
    if (!context->bc->availableRegistersRC2.empty())
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
        onlyOne = true;
        resultRegisterRC += reserveRegisterRC(context);
    }

    if (resultRegisterRC[1] != resultRegisterRC[0] + 1)
    {
        RegisterList r0;
        reserveLinearRegisterRC2(context, r0);
        emitInstruction(context, ByteCodeOp::CopyRBtoRA, r0[0], resultRegisterRC[0]);
        if (!onlyOne)
            emitInstruction(context, ByteCodeOp::CopyRBtoRA, r0[1], resultRegisterRC[1]);
        freeRegisterRC(context, resultRegisterRC);
        resultRegisterRC = r0;
    }
}

void ByteCodeGenJob::truncRegisterRC(ByteCodeGenContext* context, RegisterList& rc, int count)
{
    if (rc.size() == count)
        return;

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
        freeRegisterRC(context, rc[i]);

    rc.clear();
}

void ByteCodeGenJob::freeRegisterRC(ByteCodeGenContext* context, uint32_t rc)
{
#ifdef SWAG_HAS_ASSERT
    if (rc < MAX_CACHE_FREE_REG)
        SWAG_ASSERT(context->bc->regIsFree[rc] == UINT32_MAX);
    for (auto r : context->bc->availableRegistersRC)
        SWAG_ASSERT(r != rc);
#endif

    // In linear cache ?
    if (rc < MAX_CACHE_FREE_REG)
    {
        if (rc && context->bc->regIsFree[rc - 1] != UINT32_MAX)
        {
            uint32_t fi = context->bc->regIsFree[rc - 1];
            uint32_t bi = context->bc->availableRegistersRC.back();
            if (bi < MAX_CACHE_FREE_REG)
                context->bc->regIsFree[bi] = fi;
            context->bc->regIsFree[rc - 1]        = UINT32_MAX;
            context->bc->availableRegistersRC[fi] = bi;
            context->bc->availableRegistersRC.pop_back();

            context->bc->availableRegistersRC2.push_back(rc);
            context->bc->availableRegistersRC2.push_back(rc - 1);
            return;
        }

        if (rc < MAX_CACHE_FREE_REG - 1 && context->bc->regIsFree[rc + 1] != UINT32_MAX)
        {
            uint32_t fi = context->bc->regIsFree[rc + 1];
            uint32_t bi = context->bc->availableRegistersRC.back();
            if (bi < MAX_CACHE_FREE_REG)
                context->bc->regIsFree[bi] = fi;
            context->bc->regIsFree[rc + 1]        = UINT32_MAX;
            context->bc->availableRegistersRC[fi] = bi;
            context->bc->availableRegistersRC.pop_back();

            context->bc->availableRegistersRC2.push_back(rc + 1);
            context->bc->availableRegistersRC2.push_back(rc);
            return;
        }

        context->bc->regIsFree[rc] = (uint32_t) context->bc->availableRegistersRC.size();
    }

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
    ins.node                 = context->forceNode ? context->forceNode : node;

    if (context->noLocation)
        ins.location = nullptr;
    else if (context->forceLocation)
        ins.location = context->forceLocation;
    else if (node->parent && node->parent->kind == AstNodeKind::FuncDecl && node->kind == AstNodeKind::Statement)
        ins.location = &((AstFuncDecl*) node->parent)->content->token.endLocation;
    else
        ins.location = &node->token.startLocation;

    switch (op)
    {
    case ByteCodeOp::Jump:
    case ByteCodeOp::JumpIfTrue:
    case ByteCodeOp::JumpIfFalse:
    case ByteCodeOp::JumpIfNotZero32:
    case ByteCodeOp::JumpIfNotZero64:
    case ByteCodeOp::JumpIfZero32:
    case ByteCodeOp::JumpIfZero64:
        bc->numJumps++;
        break;

    case ByteCodeOp::IntrinsicPrintS64:
    case ByteCodeOp::IntrinsicPrintF64:
        context->bc->maxCallParams = max(context->bc->maxCallParams, 1); // Runtime call
        break;

    case ByteCodeOp::IntrinsicS8x1:
    case ByteCodeOp::IntrinsicS16x1:
    case ByteCodeOp::IntrinsicS32x1:
    case ByteCodeOp::IntrinsicS64x1:
    case ByteCodeOp::IntrinsicF32x1:
    case ByteCodeOp::IntrinsicF64x1:
    case ByteCodeOp::IntrinsicPrintString:
    case ByteCodeOp::IntrinsicCStrLen:
        context->bc->maxCallParams = max(context->bc->maxCallParams, 2); // Runtime call
        break;

    case ByteCodeOp::IntrinsicF32x2:
    case ByteCodeOp::IntrinsicF64x2:
    case ByteCodeOp::IntrinsicInterfaceOf:
    case ByteCodeOp::IntrinsicError:
    case ByteCodeOp::IntrinsicAssertMsg:
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
    case ByteCodeOp::IntrinsicAssert:
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
                    job->setPending(funcDecl->resolvedSymbolName, "AST_FULL_RESOLVE", funcDecl, nullptr);
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
                job->setPending(funcDecl->resolvedSymbolName, "AST_FULL_RESOLVE", funcDecl, nullptr);
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
    if (!(node->flags & AST_BYTECODE_GENERATED))
    {
        if (flags & ASKBC_WAIT_DONE)
        {
            SWAG_ASSERT(job);
            job->setPending(nullptr, "ASKBC_WAIT_DONE", node, nullptr);
        }

        if (!node->byteCodeJob)
        {
            node->byteCodeJob             = g_Pool_byteCodeGenJob.alloc();
            node->byteCodeJob->sourceFile = sourceFile;
            node->byteCodeJob->module     = sourceFile->module;
            if (flags & ASKBC_WAIT_DONE)
                node->byteCodeJob->dependentJob = job;
            else
                node->byteCodeJob->dependentJob = job->dependentJob;
            node->byteCodeJob->context.expansionNode = job->baseContext->expansionNode;
            node->byteCodeJob->nodes.push_back(node);
            node->bc             = g_Allocator.alloc<ByteCode>();
            node->bc->node       = node;
            node->bc->sourceFile = node->sourceFile;
            if (node->flags & AST_DEFINED_INTRINSIC)
                node->bc->name = node->token.text;
            else if (node->sourceFile->isRuntimeFile)
                node->bc->name = node->token.text.c_str();
            else
                node->bc->name = node->ownerScope->getFullName() + "_" + node->token.text.c_str();
            node->bc->name.replaceAll('.', '_');
            if (node->kind == AstNodeKind::FuncDecl)
                sourceFile->module->addByteCodeFunc(node->bc);
            if (flags & ASKBC_WAIT_DONE)
                job->jobsToAdd.push_back(node->byteCodeJob);
            else
                g_ThreadMgr.addJob(node->byteCodeJob);
            return;
        }

        if (flags & ASKBC_WAIT_DONE)
        {
            scoped_lock lk1(node->byteCodeJob->mutexDependent);
            node->byteCodeJob->dependentJobs.add(job);
        }

        return;
    }

    if (flags & ASKBC_WAIT_RESOLVED)
    {
        SWAG_ASSERT(job);
        if (!(node->flags & AST_BYTECODE_RESOLVED))
        {
            SWAG_ASSERT(node->byteCodeJob);
            node->byteCodeJob->dependentJobs.add(job);
            job->setPending(nullptr, "ASKBC_WAIT_RESOLVED", node, nullptr);
            return;
        }
    }
}

JobResult ByteCodeGenJob::execute()
{
    scoped_lock lkExecute(executeMutex);
    if (sourceFile->module->numErrors)
        return JobResult::ReleaseJob;

    if (!originalNode)
    {
        SWAG_ASSERT(nodes.size() == 1);
        originalNode = nodes.front();
    }

    SWAG_ASSERT(originalNode->byteCodeJob);
    baseContext        = &context;
    context.job        = this;
    context.sourceFile = sourceFile;
    context.bc         = originalNode->bc;
    context.node       = originalNode;

    if (pass == Pass::Generate)
    {
        // Register SystemAllocator interface to the default bytecode context
        if (sourceFile->isBootstrapFile && (originalNode->token.text == "SystemAllocator"))
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
            if (sourceFile->numErrors)
                return JobResult::ReleaseJob;

            auto node      = nodes.back();
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
        {
            emitInstruction(&context, ByteCodeOp::End);

            if (originalNode->kind == AstNodeKind::FuncDecl || context.bc->compilerGenerated)
            {
                if (g_CommandLine.stats)
                    g_Stats.numInstructions += context.bc->numInstructions;

                // Print resulting bytecode
                if (originalNode && originalNode->attributeFlags & ATTRIBUTE_PRINT_BC)
                {
                    unique_lock lk(module->mutexByteCode);
                    module->byteCodePrintBC.push_back(context.bc);
                }
            }
        }

        // Byte code is generated (but not yet resolved, as we need all dependencies to be resolved too)
        {
            unique_lock lk(originalNode->mutex);
            SWAG_ASSERT(originalNode->byteCodeJob);
            originalNode->flags |= AST_BYTECODE_GENERATED;
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
            if (node->flags & AST_BYTECODE_GENERATED)
            {
                dependentNodesTmp.pop_back();
                continue;
            }

            scoped_lock lk1(node->byteCodeJob->mutexDependent);
            node->byteCodeJob->dependentJobs.add(this);
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
                if (node->flags & AST_BYTECODE_RESOLVED)
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
                if (!dep->byteCodeJob)
                {
                    continue;
                }

                if (dep->byteCodeJob == this)
                {
                    mustWait = false;
                    break;
                }

                if (dep->byteCodeJob->dependentNodes.empty())
                    continue;

                shared_lock lk1(dep->byteCodeJob->mutexDependent);
                for (auto newDep : dep->byteCodeJob->dependentNodes)
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
            if (node->flags & AST_BYTECODE_RESOLVED)
            {
                dependentNodesTmp.pop_back();
                continue;
            }

            scoped_lock lk1(node->byteCodeJob->mutexDependent);
            node->byteCodeJob->dependentJobs.add(this);
            return JobResult::KeepJobAlive;
        }
    }

    // Inform dependencies that everything is done
    {
        unique_lock lk(originalNode->mutex);
        originalNode->flags |= AST_BYTECODE_RESOLVED;
        SWAG_ASSERT(originalNode->flags & AST_BYTECODE_RESOLVED);
        originalNode->byteCodeJob = nullptr;
    }

    // #ast can have a #[swag.printbc]. We need to print it now, because it's compile time, and the legit
    // pipeline for printing (after bc optimize) will not be called in that case
    if (originalNode->attributeFlags & ATTRIBUTE_AST_FUNC && originalNode->attributeFlags & ATTRIBUTE_PRINT_BC)
        context.bc->print();

    return JobResult::ReleaseJob;
}