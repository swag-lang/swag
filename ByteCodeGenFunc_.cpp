#include "pch.h"
#include "Global.h"
#include "AstNode.h"
#include "ByteCodeNodeId.h"
#include "ByteCodeGenJob.h"
#include "TypeInfo.h"
#include "SymTable.h"
#include "SourceFile.h"
#include "PoolFactory.h"
#include "ThreadManager.h"

bool ByteCodeGenJob::emitReturn(ByteCodeGenContext* context)
{
    AstNode* node       = context->node;
    auto     sourceFile = context->sourceFile;
    emitInstruction(context, ByteCodeNodeId::Ret);
    return true;
}

bool ByteCodeGenJob::emitLocalFuncCall(ByteCodeGenContext* context)
{
    AstNode* node       = context->node;
    auto     sourceFile = context->sourceFile;
    auto     overload   = node->resolvedSymbolOverload;
    auto     overnode   = CastAst<AstFuncDecl>(overload->node, AstNodeKind::FuncDecl);

    {
        scoped_lock lk(overnode->mutex);

        // Need to wait for function full semantic resolve
        if (!(overnode->flags & AST_FULL_RESOLVE))
        {
            overnode->dependentJobs.push_back(context->job);
            context->result = ByteCodeResult::Pending;
            return true;
        }

        // Need to generate bytecode, if not already done or running
        if (!(overnode->flags & AST_BYTECODE_GENERATED))
        {
            if (!overnode->byteCodeJob)
            {
                overnode->byteCodeJob               = sourceFile->poolFactory->bytecodeJob.alloc();
                overnode->byteCodeJob->sourceFile   = sourceFile;
                overnode->byteCodeJob->originalNode = overnode;
                overnode->byteCodeJob->nodes.push_back(overnode);
                g_ThreadMgr.addJob(overnode->byteCodeJob);
            }

            overnode->byteCodeJob->dependentJobs.push_back(context->job);
            context->result = ByteCodeResult::Pending;
            return true;
        }
    }

    auto& out = context->bc->out;
    emitInstruction(context, ByteCodeNodeId::LocalFuncCall);
    out.addPointer(overnode->bc);

    return true;
}
