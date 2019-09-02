#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCode.h"
#include "Global.h"
#include "Diagnostic.h"
#include "ThreadManager.h"
#include "SourceFile.h"
#include "ByteCodeOp.h"
#include "TypeInfo.h"
#include "Module.h"
#include "Attribute.h"
#include "Scope.h"
#include "Ast.h"

Pool<ByteCodeGenJob> g_Pool_byteCodeGenJob;

bool ByteCodeGenJob::internalError(ByteCodeGenContext* context, const char* msg, AstNode* node)
{
    if (!node)
        node = context->node;
    context->sourceFile->report({context->sourceFile, node->token, format("internal error, %s", msg)});
    return false;
}

uint32_t ByteCodeGenJob::reserveRegisterRC(ByteCodeGenContext* context)
{
    if (!context->bc->availableRegistersRC.empty())
    {
        auto result = context->bc->availableRegistersRC.back();
        context->bc->usedRegisters.insert(result);
        context->bc->availableRegistersRC.pop_back();
        context->bc->reservedRC.insert(result);
        return result;
    }

    auto result = context->bc->maxReservedRegisterRC++;
    context->bc->usedRegisters.insert(result);
    context->bc->reservedRC.insert(result);
    return result;
}

void ByteCodeGenJob::reserveRegisterRC(ByteCodeGenContext* context, RegisterList& rc, int num)
{
    rc.clear();
    while (num--)
    {
        rc += reserveRegisterRC(context);
    }
}

void ByteCodeGenJob::freeRegisterRC(ByteCodeGenContext* context, RegisterList& rc)
{
    for (int i = 0; i < rc.size(); i++)
    {
        context->bc->reservedRC.erase(rc[i]);
        freeRegisterRC(context, rc[i]);
    }

    rc.clear();
}

void ByteCodeGenJob::freeRegisterRC(ByteCodeGenContext* context, uint32_t rc)
{
    context->bc->reservedRC.erase(rc);
#ifdef _DEBUG
    for (auto r : context->bc->availableRegistersRC)
        SWAG_ASSERT(r != rc);
#endif
    context->bc->availableRegistersRC.push_back(rc);
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
    ins.a.u32                = r0;
    ins.b.u32                = r1;
    ins.c.u32                = r2;
    ins.sourceFileIdx        = node->sourceFileIdx;
    ins.startLocation        = node->token.startLocation;
    ins.endLocation          = node->token.endLocation;
    return &ins;
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

JobResult ByteCodeGenJob::execute()
{
    if (!syncToDependentNodes)
    {
        ByteCodeGenContext context;

        context.job        = this;
        context.sourceFile = sourceFile;
        context.bc         = originalNode->bc;
        context.node       = originalNode;

        // Special auto generated functions
        if (originalNode->name == "opInit")
        {
            auto funcNode   = CastAst<AstFuncDecl>(originalNode, AstNodeKind::FuncDecl);
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(funcNode->parameters->childs[0]->typeInfo, TypeInfoKind::Struct);
            generateStructInit(&context, typeStruct);
        }
        else
        {
            if (!context.bc)
            {
                setupBC(sourceFile->module, originalNode);
                context.bc = originalNode->bc;
            }

            while (!nodes.empty())
            {
                auto node    = nodes.back();
                context.node = node;

                switch (node->bytecodeState)
                {
                case AstNodeResolveState::Enter:
                    node->bytecodeState = AstNodeResolveState::ProcessingChilds;

                    if (node->byteCodeBeforeFct && !node->byteCodeBeforeFct(&context))
                        return JobResult::ReleaseJob;

                    if (!(node->flags & AST_VALUE_COMPUTED) && !node->childs.empty())
                    {
                        if (!(node->flags & AST_NO_BYTECODE_CHILDS) && !(node->flags & AST_NO_BYTECODE))
                        {
                            for (int i = (int) node->childs.size() - 1; i >= 0; i--)
                            {
                                auto child = node->childs[i];
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
                            if (node->typeInfo->kind == TypeInfoKind::TypeList)
                            {
                                if (!emitExpressionList(&context))
                                    return JobResult::ReleaseJob;
                            }
                            else
                            {
                                if (!emitLiteral(&context))
                                    return JobResult::ReleaseJob;
                            }
                        }
                        else if (node->byteCodeFct)
                        {
                            context.node   = node;
                            context.result = ByteCodeResult::Done;

                            if (!node->byteCodeFct(&context))
                                return JobResult::ReleaseJob;
                            if (context.result == ByteCodeResult::Pending)
                                return JobResult::KeepJobAlive;
                        }

                        if (node->byteCodeAfterFct && !node->byteCodeAfterFct(&context))
                            return JobResult::ReleaseJob;
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
        for (auto job : dependentJobs)
            g_ThreadMgr.addJob(job);
        dependentJobs.clear();
    }

	// RC registers
	if(originalNode->bc->maxReservedRegisterRC)
		originalNode->bc->registersRC = (Register*) malloc(originalNode->bc->maxReservedRegisterRC * sizeof(Register));

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
            node->byteCodeJob->dependentJobs.push_back(this);
            return JobResult::KeepJobAlive;
        }
    }

    // Inform dependencies that everything is done
    {
        scoped_lock lk(originalNode->mutex);
        originalNode->byteCodeJob = nullptr;
        originalNode->flags |= AST_BYTECODE_RESOLVED;
        for (auto job : dependentJobs)
            g_ThreadMgr.addJob(job);
    }

    return JobResult::ReleaseJob;
}
