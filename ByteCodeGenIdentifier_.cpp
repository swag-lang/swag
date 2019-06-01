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

bool ByteCodeGenJob::emitIdentifier(ByteCodeGenContext* context)
{
    AstNode* node       = context->node;
    auto     sourceFile = context->sourceFile;

    // For a function, need to be sure that function has bytecode
    if (node->resolvedSymbolName->kind == SymbolKind::Function)
    {
        auto overload = node->resolvedSymbolOverload;
        {
            scoped_lock lk(overload->node->mutex);
            auto        overnode = CastAst<AstFuncDecl>(overload->node, AstNodeKind::FuncDecl);

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
                    overnode->byteCodeJob->dependentJobs.push_back(context->job);
                    g_ThreadMgr.addJob(overnode->byteCodeJob);
                }

                context->result = ByteCodeResult::Pending;
                return true;
            }
        }
    }

    return true;
}
