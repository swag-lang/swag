#include "pch.h"
#include "Global.h"
#include "AstNode.h"
#include "ByteCodeOp.h"
#include "ByteCodeGenJob.h"
#include "TypeInfo.h"
#include "SymTable.h"
#include "SourceFile.h"
#include "PoolFactory.h"
#include "ThreadManager.h"
#include "LanguageSpec.h"

bool ByteCodeGenJob::emitReturn(ByteCodeGenContext* context)
{
    emitInstruction(context, ByteCodeOp::Ret);
    return true;
}

bool ByteCodeGenJob::emitIntrinsic(ByteCodeGenContext* context)
{
    AstNode* node       = context->node;
    auto     overload   = node->resolvedSymbolOverload;
    auto     typeInfo   = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FunctionAttribute);
    auto     callParams = CastAst<AstNode>(node->childs[0], AstNodeKind::FuncCallParams);
    switch (typeInfo->intrinsic)
    {
    case Intrisic::Print:
        switch (callParams->childs[0]->typeInfo->nativeType)
        {
        case NativeType::S64:
            emitInstruction(context, ByteCodeOp::IntrinsicPrintS64);
            break;
        case NativeType::F64:
            emitInstruction(context, ByteCodeOp::IntrinsicPrintF64);
            break;
        case NativeType::Char:
            emitInstruction(context, ByteCodeOp::IntrinsicPrintChar);
            break;
        case NativeType::String:
            emitInstruction(context, ByteCodeOp::IntrinsicPrintString);
            break;
        }
        break;
    case Intrisic::Assert:
        emitInstruction(context, ByteCodeOp::IntrinsicAssert);
        break;
    default:
        assert(false);
    }

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

    emitInstruction(context, ByteCodeOp::LocalFuncCall)->r0.pointer = overnode->bc;
    return true;
}
