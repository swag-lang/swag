#include "pch.h"
#include "ByteCodeGenJob.h"
#include "SourceFile.h"
#include "Global.h"
#include "PoolFactory.h"
#include "Diagnostic.h"
#include "ThreadManager.h"

bool ByteCodeGenJob::internalError(ByteCodeGenContext* context)
{
    AstNode* node = context->node;
    context->sourceFile->report({context->sourceFile, node->token, "internal compiler error during bytecode generation"});
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

    ByteCodeInstruction& ins = bc->out[bc->numInstructions++];
    ins.op                   = op;
    ins.r0.u32               = r0;
    ins.r1.u32               = r1;
    ins.r2.u32               = r2;
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
        originalNode->bc = context.bc = sourceFile->poolFactory->byteCode.alloc();

    while (!nodes.empty())
    {
        auto node    = nodes.back();
        context.node = node;

        switch (node->bytecodeState)
        {
        case AstNodeResolveState::Enter:
            node->bytecodeState = AstNodeResolveState::ProcessingChilds;
            if (!(node->flags & AST_VALUE_COMPUTED) && !node->childs.empty())
            {
                for (int i = (int) node->childs.size() - 1; i >= 0; i--)
                {
                    auto child = node->childs[i];
                    nodes.push_back(child);
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
                context.node = node;
                if (!node->byteCodeFct(&context))
                    return JobResult::ReleaseJob;
                if (context.result == ByteCodeResult::Pending)
                    return JobResult::KeepJobAlive;
            }

            nodes.pop_back();
            break;
        }
    }

    emitInstruction(&context, ByteCodeOp::End);

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
