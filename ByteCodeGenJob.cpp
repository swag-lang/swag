#include "pch.h"
#include "ByteCodeGenJob.h"
#include "SourceFile.h"
#include "AstNode.h"
#include "Global.h"
#include "ByteCode.h"
#include "SourceFile.h"
#include "PoolFactory.h"
#include "Diagnostic.h"
#include "ThreadManager.h"

bool ByteCodeGenJob::internalError(ByteCodeGenContext* context)
{
    AstNode* node = context->node;
    context->sourceFile->report({context->sourceFile, node->token, "internal compiler error during bytecode generation"});
    return false;
}

void ByteCodeGenJob::emitInstruction(ByteCodeGenContext* context, ByteCodeOp id)
{
    AstNode* node = context->node;
    if (context->debugInfos && node && node->sourceFileIdx != UINT32_MAX)
    {
        context->bc->out.addU16((uint16_t) ByteCodeOp::DebugInfos);
        context->bc->out.addU32(node->sourceFileIdx);
        context->bc->out.addU32(node->token.startLocation.line);
        context->bc->out.addU32(node->token.startLocation.column);
        context->bc->out.addU32(node->token.startLocation.seekStartLine);
        context->bc->out.addU32(node->token.endLocation.line);
        context->bc->out.addU32(node->token.endLocation.column);
        context->bc->out.addU32(node->token.endLocation.seekStartLine);
    }

    context->bc->out.addU16((uint16_t) id);
}

JobResult ByteCodeGenJob::execute()
{
    ByteCodeGenContext context;
    context.job        = this;
    context.sourceFile = sourceFile;
    context.bc         = originalNode->bc;
    if (!context.bc)
        originalNode->bc = context.bc = sourceFile->poolFactory->byteCode.alloc();
    context.bc->hasDebugInfos = context.debugInfos;

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
