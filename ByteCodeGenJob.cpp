#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCode.h"
#include "Diagnostic.h"
#include "ThreadManager.h"
#include "SourceFile.h"
#include "ByteCodeOp.h"
#include "Module.h"
#include "Attribute.h"
#include "Scope.h"
#include "Ast.h"
#include "Stats.h"
#include "TypeManager.h"
#include "Workspace.h"
#include "Context.h"

Pool<ByteCodeGenJob> g_Pool_byteCodeGenJob;

bool ByteCodeGenJob::internalError(ByteCodeGenContext* context, const char* msg, AstNode* node)
{
    if (!node)
        node = context->node;
    context->sourceFile->report({context->sourceFile, node, format("internal error, %s", msg)});
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

bool ByteCodeGenJob::emitPassThrough(ByteCodeGenContext* context)
{
    auto node              = context->node;
    node->resultRegisterRC = node->childs.back()->resultRegisterRC;
    return true;
}

void ByteCodeGenJob::freeRegisterRC(ByteCodeGenContext* context, RegisterList& rc)
{
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

ByteCodeInstruction* ByteCodeGenJob::emitInstruction(ByteCodeGenContext* context, ByteCodeOp op, uint32_t r0, uint32_t r1, uint32_t r2)
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
    ins.cache.u64            = 0;
    ins.sourceFileIdx        = node->sourceFileIdx;
    ins.startLocation        = node->token.startLocation;
    ins.endLocation          = node->token.endLocation;

    if (g_CommandLine.stats)
        g_Stats.numInstructions++;

    return &ins;
}

void ByteCodeGenJob::inherhitLocation(ByteCodeInstruction* inst, AstNode* node)
{
    inst->startLocation = node->token.startLocation;
    inst->endLocation   = node->token.endLocation;
}

void ByteCodeGenJob::setupBC(Module* module, AstNode* node)
{
    node->bc             = g_Pool_byteCode.alloc();
    node->bc->node       = node;
    node->bc->sourceFile = module->files[node->sourceFileIdx];
    node->bc->name       = node->ownerScope->fullname + "_" + node->name;
    replaceAll(node->bc->name, '.', '_');
    if (node->kind == AstNodeKind::FuncDecl)
        module->addByteCodeFunc(node->bc);
}

void ByteCodeGenJob::setPending()
{
    context.node->byteCodePass++;
    context.result = ByteCodeResult::Pending;
}

JobResult ByteCodeGenJob::execute()
{
    scoped_lock lkExecute(executeMutex);

#ifdef SWAG_HAS_ASSERT
    g_diagnosticInfos.pass       = "ByteCodeGenJob";
    g_diagnosticInfos.sourceFile = sourceFile;
    g_diagnosticInfos.node       = originalNode;
#endif

    if (!syncToDependentNodes)
    {
        context.job        = this;
        context.sourceFile = sourceFile;
        context.bc         = originalNode->bc;
        context.node       = originalNode;

        // Special auto generated functions
        if (originalNode->name == "opInit")
        {
            auto funcNode    = CastAst<AstFuncDecl>(originalNode, AstNodeKind::FuncDecl);
            auto typePointer = CastTypeInfo<TypeInfoPointer>(funcNode->parameters->childs[0]->typeInfo, TypeInfoKind::Pointer);
            auto typeStruct  = CastTypeInfo<TypeInfoStruct>(typePointer->finalType, TypeInfoKind::Struct);
            generateStruct_opInit(&context, typeStruct);
        }
        else
        {
            if (!context.bc)
            {
                setupBC(sourceFile->module, originalNode);
                context.bc = originalNode->bc;
            }

            // Register some default swag functions
            if (originalNode->name == "defaultAllocator" && sourceFile->swagFile)
            {
                g_defaultContext.allocator = context.bc;
            }

            while (!nodes.empty())
            {
                auto node    = nodes.back();
                context.node = node;

                switch (node->bytecodeState)
                {
                case AstNodeResolveState::Enter:
                    node->bytecodeState = AstNodeResolveState::ProcessingChilds;
                    context.result      = ByteCodeResult::Done;

                    if (node->byteCodeBeforeFct && !node->byteCodeBeforeFct(&context))
                        return JobResult::ReleaseJob;
                    SWAG_ASSERT(context.result == ByteCodeResult::Done);

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
                            SWAG_ASSERT(context.result == ByteCodeResult::Done);
                        }
                        else if (node->byteCodeFct)
                        {
                            context.node   = node;
                            context.result = ByteCodeResult::Done;

                            if (!node->byteCodeFct(&context))
                                return JobResult::ReleaseJob;
                            if (context.result == ByteCodeResult::Pending)
                                return JobResult::KeepJobAlive;
                            if (context.result == ByteCodeResult::NewChilds)
                                continue;
                        }

                        if (node->byteCodeAfterFct && !node->byteCodeAfterFct(&context))
                            return JobResult::ReleaseJob;
                        if (context.result == ByteCodeResult::NewChilds)
                            continue;
                        SWAG_ASSERT(context.result != ByteCodeResult::Pending);
                    }

                    nodes.pop_back();
                    break;
                }
            }

            emitInstruction(&context, ByteCodeOp::End);

            // Print resulting bytecode
            if (originalNode->attributeFlags & ATTRIBUTE_PRINTBYTECODE)
                context.bc->print();
        }
    }

    // Inform dependencies that this node has bytecode
    {
        scoped_lock lk(originalNode->mutex);
        originalNode->flags |= AST_BYTECODE_GENERATED;
        for (auto job : dependentJobs.list)
            g_ThreadMgr.addJob(job);
        dependentJobs.clear();
    }

    // Wait for other dependent nodes to be generated
    syncToDependentNodes = true;
    for (int i = (int) dependentNodes.size() - 1; i >= 0; i--)
    {
        auto        node = dependentNodes[i];
        scoped_lock lk(node->mutex);
        if (node->flags & AST_BYTECODE_RESOLVED)
            dependentNodes.pop_back();
        else
        {
            node->byteCodeJob->dependentJobs.add(this);
            return JobResult::KeepJobAlive;
        }
    }

    // Inform dependencies that everything is done
    {
        scoped_lock lk(originalNode->mutex);
        originalNode->byteCodeJob = nullptr;
        originalNode->flags |= AST_BYTECODE_RESOLVED;
        for (auto job : dependentJobs.list)
            g_ThreadMgr.addJob(job);
    }

    return JobResult::ReleaseJob;
}
