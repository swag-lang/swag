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

Pool<ByteCodeGenJob> g_Pool_byteCodeGenJob;

bool ByteCodeGenJob::internalError(ByteCodeGenContext* context, const char* msg)
{
    context->sourceFile->report({context->sourceFile, context->node->token, format("internal compiler error during bytecode generation (%s)", msg)});
    return false;
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

    assert(bc->out);
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

JobResult ByteCodeGenJob::execute()
{
    ByteCodeGenContext context;

    context.job        = this;
    context.sourceFile = sourceFile;
    context.bc         = originalNode->bc;
    if (!context.bc)
    {
        originalNode->bc             = g_Pool_byteCode.alloc();
        context.bc                   = originalNode->bc;
        originalNode->bc->node       = originalNode;
        originalNode->bc->sourceFile = sourceFile;
        if (originalNode->kind == AstNodeKind::FuncDecl)
            sourceFile->module->addByteCodeFunc(originalNode->bc);
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
                if (!(node->flags & AST_NO_BYTECODE_CHILDS))
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
            if (node->flags & AST_VALUE_COMPUTED)
            {
                context.node = node;
                if (!emitLiteral(&context))
                    return JobResult::ReleaseJob;
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
            nodes.pop_back();
            break;
        }
    }

    emitInstruction(&context, ByteCodeOp::End);

    // Print resulting bytecode
    if (originalNode->attributeFlags & ATTRIBUTE_PRINT_BYTECODE)
        context.bc->print();

    // Inform dependencies that this node has bytecode
    {
        scoped_lock lk(originalNode->mutex);
        originalNode->byteCodeJob = nullptr;
        originalNode->flags |= AST_BYTECODE_GENERATED;
        for (auto job : dependentJobs)
            g_ThreadMgr.addJob(job);
    }

    return JobResult::ReleaseJob;
}
